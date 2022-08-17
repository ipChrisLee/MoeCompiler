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
	'--move_sy',
	action='store',
	nargs='?',
	default=None,
	type=str,
	help='Move testcase and test then.'
)
argParser.add_argument(
	'--difftest',
	action='store_true',
	default=False,
	help='Do a diff test with gcc.'
)
argParser.add_argument(
	'--moeOpti',
	action='store',
	nargs=1,
	type=int,
	default=0,
	dest='moeOpti',
	help='Opti level when using MoeCompiler.',
)
argParser.add_argument(
	'--frontend',
	action='store_true',
	dest='frontend',
	help='Just test frontend of MoeCompiler.\n'
	     '(.sy =(moe)=> .mll =(llc)=> .ms =(pi)=> .res.json)'
)
argParser.add_argument(
	'--moe',
	action='store_true',
	dest='moe',
	help='Just test moe.\n(.sy =(moe)=> .ms =(pi)=> .res.json)'
)
argParser.add_argument(
	'--gen',
	action='store_true',
	dest='gen',
	help='Just use moe to compile.\n(.sy =(moe)=> .mll && .sy =(moe)=> .ms)'
)
argParser.add_argument(
	'--llvm',
	action='store_true',
	dest='llvm',
	help='Generate codes by using llvm tool-chain.\n'
	     '(.sy =(clang)=> .ll =(llc)=> .s =(pi)=> .res.json)'
)
argParser.add_argument(
	'--asm_run',
	action='store_true',
	dest='asm_run',
	help='Run asm file.\n'
	     '(.ms =(pi)=> .res.json)'
)
argParser.add_argument(
	'--llvmir_run',
	action='store_true',
	dest='llvmir_run',
	help='Run llvm-ir file.\n'
	     '(.mll =(llc)=> .ms =(pi)=> .res.json)'
)
argParser.add_argument(
	'--gcc_table',
	action='store_true',
	dest='gcc_table',
	help='Generate gcc table.'
)


def test_frontend():
	res = Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=True, emit_dessa=False,
		float_dec_format=False
	)
	if res.returncode != 0:
		cprint(f'{res.stderr}', color=C.WA)
		exit(1)
	res = LLC.compile_llvmir(
		llFilePath=TestFilesSettings.FilePath.testMLL,
		sFilePath=TestFilesSettings.FilePath.testMS
	)
	if res.returncode != 0:
		cprint(f'{res.stderr}', color=C.WA)
		exit(1)
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testMS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes
	)
	print(f'Result : {res.test_status.value}')


def test_gen():
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=False, emit_dessa=True,
		float_dec_format=False
	).check_returncode()
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMS,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=False, emit_dessa=False,
		float_dec_format=False
	).check_returncode()
	


def test_moe():
	Moe.compile(
		syFilePath=TestFilesSettings.FilePath.testSy,
		msFilePath=TestFilesSettings.FilePath.testMLL,
		optiLevel=args.moeOpti, timeout=TimeoutSettings.moe, emit_llvm=False, emit_dessa=True,
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
		passes=['mem2reg']
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
	res = LLC.compile_llvmir(
		llFilePath=TestFilesSettings.FilePath.testMLL,
		sFilePath=TestFilesSettings.FilePath.testMS
	)
	if res.returncode != 0:
		cprint(f'Error when LLC compiling: \n{res.stderr}\n', color=C.WA)
		exit(1)
	res = Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testMS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes,
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
			return
	for testCase in allPerformanceTestsCaseSet.caseSet:
		if testCase._testName == name:
			testCase.copy_to_test_files()
			cprint(f'Test [{name}] is found. Copying to test files.', color=C.INFO)
			return
	cprint(f'Test [{name}] not found! Terming tests_gen.', color=C.ERR)
	exit(1)


def generate_std():
	res = GCC.compile_to_ass(
		syFilePath=TestFilesSettings.FilePath.testSy,
		sFilePath=TestFilesSettings.FilePath.testS,
		optiLevel=2
	)
	if res.returncode:
		cprint(f'Compile Error on GCC: [{res.stderr}]', color=C.WA)
		exit(1)
	Pi.run_tester(
		sFilePath=TestFilesSettings.FilePath.testS,
		inFilePath=TestFilesSettings.FilePath.testIn,
		outFilePath=TestFilesSettings.FilePath.testOut,
		resFilePath=TestFilesSettings.FilePath.testRes,
		mySysYLib=True
	)
	Pi.get_from_pi('.tmp/buffer.txt', TestFilesSettings.FilePath.testOut)


if __name__ == '__main__':
	args = argParser.parse_args()
	argParser.print_help()
	if args.move_sy is not None:
		move_sy(args.move_sy)
	if args.difftest:
		generate_std()
	
	if args.frontend:
		test_frontend()
	elif args.moe:
		test_moe()
	elif args.llvm:
		test_llvm()
	elif args.asm_run:
		test_asm_run()
	elif args.llvmir_run:
		test_llvmir_run()
	elif args.gcc_table:
		test_gcc_table()
	elif args.gen:
		test_gen()
