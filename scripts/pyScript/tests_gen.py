from scripts.pyScript.entities.Moe import Moe
from scripts.pyScript.entities.llvm import Clang, Opt, LLC
from scripts.pyScript.entities.Pi import Pi
from scripts.pyScript.entities.gcc import GCC
from scripts.pyScript.case_and_case_set import allPerformanceTestsCaseSet, \
	myFuncTestCaseSet, allFunctionTestsCaseSet
from scripts.pyScript.helper.settings import TestFilesSettings, TimeoutSettings, \
	bufferTextFilePath, JudgmentSettings
from scripts.piScripts.test_info import RunningInfo
import argparse
from scripts.pyScript.helper.color import cprint, C
import json
import tabulate
import typing as typ

argParser = argparse.ArgumentParser(
	description='Use clang and moe compile SysY.',
	add_help=True
)
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
	'--moe',
	action='store_true',
	dest='moe',
	help='Just test moe.'
)
argParser.add_argument(
	'--asm_run',
	action='store_true',
	dest='asm_run',
	help='Run asm file.'
)
argParser.add_argument(
	'--llvmir-run',
	action='store_true',
	dest='llvmir_run',
	help='Run llvm-ir file.'
)
argParser.add_argument(
	'--difftest',
	action='store_true',
	dest='difftest',
	help='Difftest between clang and moe.'
)
argParser.add_argument(
	'--backend',
	action='store_true',
	dest='backend',
	help='Test backend (almost test moe).'
)
argParser.add_argument(
	'--gcc_table',
	action='store_true',
	dest='gcc_table',
	help='Generate gcc table.'
)
argParser.add_argument(
	'--move_sy',
	nargs=1,
	default=None,
	type=str
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
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testMS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	print(f'Result : {res.test_status.value}')


def test_moe():
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=True,
		float_dec_format=False
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
	print(f'Result : {res.test_status.value}')


def test_llvm():
	res = Clang.compile_to_llvmir(
		syFilePath=TestFilesSettings.FilePath.testSy,
		llFilePath=TestFilesSettings.FilePath.testLL
	)
	if res.returncode != 0:
		cprint(res.stderr, color=C.WA)
		exit(1)
	Opt.opt(
		llFilePath=TestFilesSettings.FilePath.testLL,
		newLLFilePath=TestFilesSettings.FilePath.testOptLL,
		passes=['-mem2reg']
	).check_returncode()
	LLC.compile_llvmir(
		llFilePath=TestFilesSettings.FilePath.testLL,
		sFilePath=TestFilesSettings.FilePath.testS
	).check_returncode()
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	print(f'Result : {res.test_status.value}')


def test_asm_run():
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	print(f'Result : {res.test_status.value}')


def test_llvmir_run():
	LLC.compile_llvmir(
		llFilePath=TestFilesSettings.FilePath.testLL,
		sFilePath=TestFilesSettings.FilePath.testS
	).check_returncode()
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes,
	)
	print(f'Result : {res.test_status.value}')


def test_difftest():
	res = Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=True,
		float_dec_format=False
	)
	if res.returncode != 0:
		cprint(res.stderr, color=C.WA)
		res.check_returncode()
	res = Clang.compile_to_ass(
		syFilePath=TestFilesSettings.FilePath.testSy,
		sFilePath=TestFilesSettings.FilePath.testS,
		optiLevel=0
	)
	if res.returncode != 0:
		cprint(res.stderr, color=C.WA)
		res.check_returncode()
	Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	Pi.get_from_pi('.tmp/buffer.txt', TestFilesSettings.FilePath.testOut)
	res = Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMS,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=False,
		float_dec_format=False
	)
	if res.returncode != 0:
		cprint(res.stderr, color=C.WA)
		res.check_returncode()
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testMS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	print(f'Result : {res.test_status.value}')


def test_gcc_table():
	data: typ.Dict[str, dict] = {}
	table: typ.List = []
	for testCase in allPerformanceTestsCaseSet.caseSet:
		res = GCC.compile_to_ass(
			syFilePath=testCase.syFilePath,
			sFilePath=testCase.msFilePath,
			optiLevel=2
		)
		if res.returncode != 0:
			cprint(f'ErrInfo: {res.stderr}', color=C.ERR)
			exit(1)
		res = Pi.run_tester(
			sFilePath=testCase.msFilePath,
			inFilePath=testCase.inFilePath,
			outFilePath=testCase.outFilePath,
			resFilePath=bufferTextFilePath,
			mySysYLib=True
		)
		if res.exit_code != 0:
			cprint(f'ErrInfo: {res.stderr}', color=C.ERR)
			exit(1)
		data[str(testCase)] = res.to_dict()
		row = res.to_list_info(str(testCase), res.time_cost)
		table.append(row)
	with open(str(JudgmentSettings.gccPerfDataFilePath), 'w') as fp:
		fp.write(json.dumps(data, indent=4))
	with open(str(JudgmentSettings.gccPerfTableFilePath), 'w') as fp:
		fp.write(
			tabulate.tabulate(
				table, headers=RunningInfo.header, tablefmt='fancy_grid'
			)
		)


def move_sy(name: str):
	for testCase in allFunctionTestsCaseSet.caseSet:
		if testCase._testName == name:
			testCase.copy_to_test_files()
			cprint(f'Test [{name}] is found. Copying to test files.', color=C.INFO)
	for testCase in allPerformanceTestsCaseSet.caseSet:
		if testCase._testName == name:
			testCase.copy_to_test_files()
			cprint(f'Test [{name}] is found. Copying to test files.', color=C.INFO)


if __name__ == '__main__':
	args = argParser.parse_args()
	if args.move_sy is not None:
		move_sy(args.move_sy[0])
	if args.frontend:
		test_frontend()
	elif args.llvm:
		test_llvm()
	elif args.moe:
		test_moe()
	elif args.asm_run:
		test_asm_run()
	elif args.llvmir_run:
		test_llvmir_run()
	elif args.difftest:
		test_difftest()
	elif args.gcc_table:
		test_gcc_table()
