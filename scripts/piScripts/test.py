#   This is running on PI!
import argparse
import json
import subprocess
from pathlib import Path
from datetime import datetime
import atexit
from enum import Enum
from test_info import TestStatus, RunningInfo
import typing as typ
from datetime import datetime

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

exeFilePath = str(Path.home() / '.tmp/main')
bufferFilePath = str(Path.home() / '.tmp/buffer.txt')

runningInfo: typ.Optional[RunningInfo] = None


def saving_running_info_to_file():
	global runningInfo
	if runningInfo is None:
		runningInfo = RunningInfo(
			exit_code=255,
			info='Exit out of expectation when running test.py .',
			test_status=TestStatus.CE
		)
	runningInfo.time_now = str(datetime.now().strftime("%H:%M:%S"))
	with open(args.resPath, 'w') as fp:
		fp.write(json.dumps(runningInfo.to_dict(), indent=4))


atexit.register(saving_running_info_to_file)


def finish_and_dump_json(
	exitCode: int, info: str, testStatus: TestStatus,
	message: str = None, stderr: str = None, out: str = None, time_cost=None
):
	global runningInfo
	runningInfo = RunningInfo(
		exit_code=exitCode, info=info, test_status=testStatus,
		message=message, out=out, stderr=stderr, time_cost=time_cost)
	exit(0)


with open(bufferFilePath, 'w') as _:
	pass
_ = subprocess.run(
	[
		'gcc-7', '-march=armv7-a', '-O0', '-g',
		'-Wl,--whole-archive', args.aLibPath, '-Wl,--no-whole-archive',
		args.msFilePath, '-o', f'{exeFilePath}'
	],
	stderr=subprocess.PIPE, text=str, encoding='utf-8'
)
if _.returncode != 0:
	finish_and_dump_json(
		exitCode=_.returncode & 0xFF,
		info='Assemble or Link Error!', testStatus=TestStatus.CE,
		stderr=_.stderr
	)


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


def from_stderr_to_time_cost(s: str):
	s = s.splitlines()[-1]
	d = datetime.strptime(s, "TOTAL: %HH-%MM-%SS-%fus")
	t = d.minute * 60 + d.second + d.microsecond / 1000000
	if t == 0.0:
		return -1
	else:
		return t


def test_main():
	syInStream = open(args.syInPath, 'r')
	try:
		runMain = subprocess.run(
			[exeFilePath],
			stdin=syInStream, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
			text=str, encoding='utf-8', timeout=300
		)
		syInStream.close()
		retCode: int = int(runMain.returncode & 0xFF)
		out = runMain.stdout + \
		      ('\n' if len(runMain.stdout) > 0 and
		               runMain.stdout[-1] != '\n' else '') + f'{retCode}\n'
		with open(args.syOutPath, 'r') as fp:
			ans = str(fp.read())
		out, ans = diff_test_pass(out, ans)
		with open(bufferFilePath, 'w') as fp:
			fp.write(out)
		if out == ans:
			t = from_stderr_to_time_cost(runMain.stderr)
			return finish_and_dump_json(
				exitCode=0, info='Test passed!',
				testStatus=TestStatus.AC,
				stderr=runMain.stderr, time_cost=t
			)
		else:
			return finish_and_dump_json(
				exitCode=runMain.returncode,
				info='Test failed! Either program exit abnormally or different on output.',
				testStatus=TestStatus.WA, message=str(diff_message(out, ans)),
				stderr=runMain.stderr, out=out
			)
	except subprocess.TimeoutExpired as e:
		return finish_and_dump_json(
			exitCode=-1, info='Time out for running exe file.',
			stderr=e.stderr, out=e.stdout, testStatus=TestStatus.TLE
		)


if __name__ == '__main__':
	test_main()
