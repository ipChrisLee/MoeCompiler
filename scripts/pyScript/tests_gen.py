from Moe import Moe
from llvm import Clang, Opt, LLC
from Pi import Pi
from settings import TestFilesSettings, BasicSettings, TimeoutSettings
import argparse
from case_and_case_set import TestCase, allTestCaseSet
import json

argParser = argparse.ArgumentParser(
	description='Use clang and moe compile SysY.',
	add_help=True
)

# argParser.add_argument(
# 	'--release',
# 	action='store_true',
# 	dest='release',
# 	help='Use release version of MoeCompiler.'
# )
argParser.add_argument(
	'--moeOpti',
	action='store',
	type=int,
	default=0,
	dest='moeOpti',
	help='Opti level when using MoeCompiler.',
)
argParser.add_argument(
	'--frontend',
	action='store_true',
	dest='frontend',
	help='Just test frontend of MoeCompiler(-emit-llvm).'
)
argParser.add_argument(
	'--llvm',
	action='store_true',
	dest='llvm',
	help='Generate codes by using llvm tool-chain.'
)
argParser.add_argument(
	'--llvm_all',
	action='store_true',
	dest='llvm_all',
	help='Run llvm on all tests'
)
argParser.add_argument(
	'--moe',
	action='store_true',
	dest='moe',
	help='Just test moe.'
)


def test_frontend():
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=True,
		float_dec_format=False
	).check_returncode()
	LLC.compile_llvmir(
		llFilePath=TestFilesSettings.FilePath.testMLL,
		sFilePath=TestFilesSettings.FilePath.testMS
	).check_returncode()
	Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testMS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)


def test_moe():
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=True,
		float_dec_format=True
	).check_returncode()
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMS,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=False,
		float_dec_format=False
	).check_returncode()
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testMS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	print(f'Result : {res["test_status"]}')


def test_llvm():
	Clang.compile_to_llvmir(
		syFilePath=TestFilesSettings.FilePath.testSy,
		llFilePath=TestFilesSettings.FilePath.testLL
	)
	LLC.compile_llvmir(
		llFilePath=TestFilesSettings.FilePath.testLL,
		sFilePath=TestFilesSettings.FilePath.testS
	)
	Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)


def test_llvm_all():
	for testCase in allTestCaseSet.caseSet:
		Clang.compile_to_ass(
			syFilePath=testCase.syFilePath,
			sFilePath=testCase.msFilePath,
			optiLevel=2
		)
		res = Pi.run_tester(
			sFilePath=testCase.msFilePath,
			inFilePath=testCase.inFilePath,
			outFilePath=testCase.outFilePath,
			resFilePath=TestFilesSettings.FilePath.testRes,
		)
		assert res['test_status'] == 'AC'


if __name__ == '__main__':
	args = argParser.parse_args()
	if args.frontend:
		test_frontend()
	elif args.llvm:
		test_llvm()
	elif args.llvm_all:
		test_llvm_all()
	elif args.moe:
		test_moe()
