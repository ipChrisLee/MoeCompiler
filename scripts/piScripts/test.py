import argparse
import json
import subprocess

argParser = argparse.ArgumentParser(
	description='Tester arguments.'
)

argParser.add_argument(
	'--ms',
	help='Specify ms file path compiled from SysY.',
	dest='msFilePath'
)

argParser.add_argument(
	'--in',
	help='Where to redirect stdin.',
	dest='syInPath'
)

argParser.add_argument(
	'--out',
	help='Answer file.',
	dest='syOutPath'
)

argParser.add_argument(
	'--func',
	help='This is a functional test.',
	action='store_true'
)

argParser.add_argument(
	'--perf',
	help='This is a performance test.',
	action='store_true'
)

argParser.add_argument(
	'--res',
	help='[optional, default=\"result.json\"] Where to save result.',
	dest='resPath'
)

args = argParser.parse_args()


def finish_and_dump_json(return_code: int, info: str, message):
	if message is None:
		message = ''
	resPath = 'result.json'
	if args.resPath is not None:
		resPath = args.resPath
	
	runningInfo = dict()
	runningInfo['return_code'] = return_code
	runningInfo['info'] = info
	runningInfo['message'] = message
	with open(resPath, 'w') as fp:
		fp.write(json.dumps(runningInfo, indent=4))
	exit(0)


exeFilePath = './main'
bufferFilePath = './buffer.txt'
syInPath = str(args.syInPath)
syAnsPath = str(args.syOutPath)

with open(bufferFilePath, 'w') as _:
	pass
_ = subprocess.run(
	['gcc-7', '-march=armv7', args.msFilePath, 'libsysy.a', '-o', f'{exeFilePath}'],
	stderr=subprocess.PIPE
)
if _.returncode != 0:
	finish_and_dump_json(
		return_code=_.returncode & 0xFF, info='Assemble or Link Error!',
		message=_.stderr
	)


def diff_test_pass(ansFilePath, outFilePath):
	strAns = open(ansFilePath, 'r').read()
	strOut = open(outFilePath, 'r').read()
	return strAns == strOut


def func_test_main():
	syInStream = open(syInPath, 'r')
	runMain = subprocess.run(
		[exeFilePath],
		stdin=syInStream, stdout=subprocess.PIPE, stderr=subprocess.PIPE
	)
	syOutStream = open(bufferFilePath, 'w')
	syOutStream.write(str(runMain.returncode & 0xFF) + '\n')
	syOutStream.flush()
	syOutStream.close()
	if not diff_test_pass(syAnsPath, bufferFilePath):
		return finish_and_dump_json(
			return_code=0xFF, info='Functional test failed!', message=''
		)
	else:
		return finish_and_dump_json(
			return_code=0, info='Performance test passed!', message=''
		)


def perf_test_main():
	syInStream = open(syInPath, 'r')
	syOutStream = open(bufferFilePath, 'w')
	runMain = subprocess.run(
		[exeFilePath],
		stdin=syInStream, stdout=syOutStream, stderr=subprocess.PIPE
	)
	syOutStream.flush()
	syOutStream.close()
	if runMain.returncode != 0:
		finish_and_dump_json(
			return_code=runMain.returncode & 0xFF,
			info='Program Exit with return code not equal to zero.[performance test]',
			message=runMain.stderr
		)
	
	if not diff_test_pass(syAnsPath, bufferFilePath):
		return finish_and_dump_json(
			return_code=0xFF, info='Performance test failed!', message=''
		)
	else:
		return finish_and_dump_json(
			return_code=0, info='Performance test passed!', message=''
		)


if __name__ == '__main__':
	if args.func:
		func_test_main()
	elif args.perf:
		perf_test_main()
	else:
		finish_and_dump_json(
			return_code=255,
			info='You should specify functional test or performance test.',
			message=''
		)
