import argparse
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
	help='Where to redirect stdout.',
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
	help='Where to save result.',
	save='resPath'
)

args = argParser.parse_args()

p = subprocess.run(['gcc-7', '-march=armv7', args.msFilePath, '-l libsysy.a'])


def func_test_main():
	pass


def perf_test_main():
	pass


if __name__ == '__main__':
	if args.func:
		func_test_main()
	elif args.perf:
		perf_test_main()
	else:
		pass
