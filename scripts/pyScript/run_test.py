from compiler_tester import TestUnit, ActionBasic, list_all_tests
from command_line_tool import RunningInfo, use_moe_compile_sy, run_command
import typing as typ
from case_and_case_set import emptyTestCaseSet, basicTestCaseSet, \
	function2020TestCaseSet, allFunctionTestsCaseSet
import argparse
from settings import TestUnitSettings, bufferTextFilePath
from color import cprint, C


class ActionJustCompileToLLVMIR(ActionBasic):
	def get_config(self) -> str:
		return f'Just compile sy file to llvm-ir, with optiLevel=' \
		       f'{self.optiLevel}.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testUnit, testCase, **kwargs) -> RunningInfo:
		if testUnit is None or testCase is None:
			exit(-1)
		return use_moe_compile_sy(
			syFilePath=testCase.syFilePath,
			msFilePath=testCase.msFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel,
			terminalVerbose=testUnit.terminalVerbose,
		)


class ActionCompileToLLVMIRAndUseLLVM(ActionBasic):
	def get_config(self) -> str:
		return f'Compile sy file to llvm-ir use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}. Use clang compile llvm-ir ' \
		       f'then.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testUnit, testCase, **kwargs) -> RunningInfo:
		if testUnit is None or testCase is None:
			exit(-1)
		rInfo = use_moe_compile_sy(
			syFilePath=testCase.syFilePath,
			msFilePath=bufferTextFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel,
			terminalVerbose=testUnit.terminalVerbose,
		)
		if rInfo.exitCode != 0:
			return rInfo
		rInfo = run_command(
			'llc', bufferTextFilePath, '-o', testCase.msFilePath
		)
		return rInfo


allTestUnits: typ.Dict[str, TestUnit] = {
	'empty': TestUnit(
		name='empty', testCaseSet=emptyTestCaseSet,
		terminalVerbose=True,
		helpInfo='Just an empty test unit.',
		actions={
			'Basic': ActionBasic()
		}
	),
	'test_frontend': TestUnit(
		name='test_frontend',
		testCaseSet=basicTestCaseSet,
		terminalVerbose=True,
		helpInfo='Test frontend.',
		actions={
			'toLLVMIR': ActionJustCompileToLLVMIR(optiLevel=0)
		}
	),
	'test_frontend_with_llc': TestUnit(
		name='test_frontend_with_llc',
		testCaseSet=basicTestCaseSet,
		terminalVerbose=True,
		helpInfo='Test frontend.',
		actions={
			'toLLVMIRAndLLC': ActionCompileToLLVMIRAndUseLLVM(optiLevel=0)
		}
	),
	'test_frontend_with_llc_on_std_test2020': TestUnit(
		name='test_frontend_with_llc_on_std_test2020',
		testCaseSet=function2020TestCaseSet,
		terminalVerbose=True,
		helpInfo='Test frontend with llc and on std_test 2020.',
		actions={
			'toLLVMIRAndLLC': ActionCompileToLLVMIRAndUseLLVM(optiLevel=0)
		}
	),
	'test_frontend_with_llc_on_all_function_test_set': TestUnit(
		name='test_frontend_with_llc_on_all_function_test_set',
		testCaseSet=allFunctionTestsCaseSet,
		terminalVerbose=True,
		helpInfo='Test frontend with llc and on functional tests.',
		actions={
			'toLLVMIRAndLLC': ActionCompileToLLVMIRAndUseLLVM(optiLevel=0)
		}
	)
}
argParser = argparse.ArgumentParser(
	description='Tester arguments.'
)
argParser.add_argument(
	'-t', '--test',
	help='Specify a test to run. [-l]/[--list] shows all supported tests.',
	dest='testName'
)
argParser.add_argument(
	'-l', '--list',
	help='List all available tests.',
	action='store_true'
)
# argParser.add_argument(
#     '--write_error_file_to_testDir_test_sy',
#     help='If error occurs, write error sy file to testDir/test.sy',
#     action='store_true'
# )
argParser.add_argument(
	'--save_result_to_table',
	help='Save test result to table.',
	action='store_true'
)
argParser.add_argument(
	'--copy_failed_test_sy',
	help=f'Save failed test sy file to {TestUnitSettings.syPathForFailedTest}.',
	action='store_true'
)
argParser.add_argument(
	'--verbose_of_terminal',
	help=f'Show every command tester runs.',
	action='store_true'
)

if __name__ == '__main__':
	clArgs = argParser.parse_args()
	if clArgs.list:
		list_all_tests(allTestUnits)
	if clArgs.testName:
		if clArgs.testName not in allTestUnits.keys():
			cprint(f'Unknown/Unsupported test: {clArgs.testName}!', color=C.ERR)
		else:
			allTestUnits[clArgs.testName].do_this_test(
				saveToTable=clArgs.save_result_to_table,
				copyFailedTestCaseToTestSy=clArgs.copy_failed_test_sy)
	cprint(f'Tester Finished!', color=C.FIN)
