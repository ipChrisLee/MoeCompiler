#   This is running on PI!
import argparse
import json
import subprocess
from pathlib import Path
from datetime import datetime
import atexit
from enum import Enum

argParser = argparse.ArgumentParser(
	description='Tester arguments.'
)

argParser.add_argument(
	'--ms',
	help='Specify ms file path compiled from SysY.',
	action='store',
	dest='msFilePath',
	type=str,
)

argParser.add_argument(
	'--in',
	help='Where to redirect stdin.',
	action='store',
	dest='syInPath',
	type=str,
)

argParser.add_argument(
	'--out',
	help='Answer file.',
	action='store',
	dest='syOutPath',
	type=str,
)
argParser.add_argument(
	'--perf',
	help='This is a performance test.',
	action='store_true',
	dest='perf'
)
argParser.add_argument(
	'--func',
	help='This is a functional test.',
	action='store_true',
	dest='func'
)
argParser.add_argument(
	'--res',
	help='Where to save result.',
	action='store',
	dest='resPath',
	type=str
)
argParser.add_argument(
	'--alib',
	help='Library path.',
	action='store',
	dest='aLibPath',
	type=str
)

args = argParser.parse_args()

assert (args.msFilePath is not None) and \
       (args.syInPath is not None) and \
       (args.syOutPath is not None) and \
       (args.resPath is not None) and \
       (args.aLibPath is not None)
# (args.func ^ args.perf) and \

exeFilePath = str(Path.home() / '.tmp/main')
bufferFilePath = str(Path.home() / '.tmp/buffer.txt')
runningInfo = dict()


class TestStatus(Enum):
	AC = 'AC'
	WA = 'WA'
	TLE = 'TLE'
	CE = 'CE'


"""
	exit_status:
		Status when leaving test.py .
	info:
		Information of exit_status test.py .
	message:
		[optional]
		Other messages except info.
	out:
		[optional]
		Stdout of executable main file and its return code.
	stderr:
		[optional]
		Stderr of executable main file.
	test_status:
		Value:
			{CE} if something happened before running main.
			{AC,WA,TLE} otherwise.
		Status of test.
	time_now:
		Time when leaving test.py .
"""


def saving_running_info_to_file():
	if not runningInfo:
		runningInfo['exit_status'] = 255
		runningInfo['info'] = 'Exit out of expectation when running test.py .'
		runningInfo['test_status'] = TestStatus.CE.value
	runningInfo['time_now'] = str(datetime.now().strftime("%H:%M:%S"))
	with open(args.resPath, 'w') as fp:
		fp.write(json.dumps(runningInfo, indent=4))


atexit.register(saving_running_info_to_file)


def finish_and_dump_json(exitStatus: int, info: str, testStatus: TestStatus,
                         message: str = None, stderr: str = None, out: str = None
                         ):
	runningInfo['exit_status'] = exitStatus
	runningInfo['info'] = info
	runningInfo['test_status'] = testStatus.value
	if message is not None:
		runningInfo['message'] = message
	if stderr is not None:
		runningInfo['stderr'] = stderr
	if out is not None:
		runningInfo['out'] = out
	exit(exitStatus)


with open(bufferFilePath, 'w') as _:
	pass
_ = subprocess.run(
	[
		'gcc-7', '-march=armv7-a', '-O0',
		'-Wl,--whole-archive', args.aLibPath, '-Wl,--no-whole-archive',
		args.msFilePath, '-o', f'{exeFilePath}'
	],
	stderr=subprocess.PIPE, text=str, encoding='utf-8'
)
if _.returncode != 0:
	finish_and_dump_json(exitStatus=_.returncode & 0xFF,
	                     info='Assemble or Link Error!', testStatus=TestStatus.CE,
	                     message=_.stderr)


def diff_test_pass(out: str, ans: str):
	newOut: str = ''
	newAns: str = ''
	for line in out.splitlines():
		newOut = newOut + line.rstrip() + '\n'
	for line in ans.splitlines():
		newAns = newAns + line.rstrip() + '\n'
	out = newOut
	ans = newAns
	while len(out) > 0 and out[-1] == '\n':
		out = out[0:-1]
	while len(ans) > 0 and ans[-1] == '\n':
		ans = ans[0:-1]
	return out, ans


def diff_message(out: str, ans: str):
	return [i for i in range(min(len(out), len(ans))) if out[i] != ans[i]] + \
	       [-1] if len(out) != len(ans) else []


def test_main():
	syInStream = open(args.syInPath, 'r')
	runMain = subprocess.run(
		[exeFilePath],
		stdin=syInStream, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
		text=str, encoding='utf-8', timeout=300
	)
	syInStream.close()
	retCode: int = int(runMain.returncode & 0xFF)
	out = runMain.stdout + \
	      ('\n' if len(runMain.stdout) > 0 and runMain.stdout[-1] != '\n' else '') + \
	      f'{retCode}\n'
	with open(args.syOutPath, 'r') as fp:
		ans = str(fp.read())
	out, ans = diff_test_pass(out, ans)
	with open(bufferFilePath, 'w') as fp:
		fp.write(out)
	if out == ans:
		return finish_and_dump_json(
			exitStatus=0, info='Test passed!',
			testStatus=TestStatus.AC,
			stderr=runMain.stderr, out=out
		)
	else:
		return finish_and_dump_json(
			exitStatus=255, info='Test failed!',
			testStatus=TestStatus.WA, message=str(diff_message(out, ans)),
			stderr=runMain.stderr, out=out
		)


if __name__ == '__main__':
	test_main()
