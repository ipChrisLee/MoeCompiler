from color import C, cprint, cprint_separate_line
from case_and_case_set import TestCaseSet, TestCase
from settings import TestUnitSettings, TestFilesSettings
import actions
import typing as typ
from case_and_case_set import emptyTestCaseSet, basicTestCaseSet, \
	function2020TestCaseSet, allFunctionTestsCaseSet, functionWithoutFloat
from subprocess import CalledProcessError
from Bash import Bash


class TestUnit:
	def __init__(
		self,
		name: str,
		testCaseSet: TestCaseSet,
		acts: typ.List[actions.ActionBasic],
		terminalVerbose: bool = True,
		helpInfo: str = 'No help info.'
	):
		self.name = name
		self.testCaseSet = testCaseSet
		self.terminalVerbose = terminalVerbose
		self.helpInfo = helpInfo
		self.actions = acts
	
	def __hash__(self):
		return hash(self.name)
	
	def __eq__(self, other):
		if not isinstance(other, TestUnit):
			return False
		return hash(self) == hash(other)
	
	def do_this_test(
		self,
		saveToTable: typ.Optional[bool],
		copyFailedTestCaseToTestSy: typ.Optional[bool]
	):
		if saveToTable is None:
			saveToTable = False
		if copyFailedTestCaseToTestSy is None:
			copyFailedTestCaseToTestSy = True
		# ''' multiple line is too ugly.
		cprint_separate_line(f'Testing', color=C.INFO)
		cprint_separate_line(f'Test info', color=C.INFO, fill_len_indent=4)
		cprint(
			f'TestUnit [{self.name}] will be tested.\n'
			f'\tTestCase = {self.testCaseSet.setName}\n'
			f'\tTestActions = ',
			color=C.INFO
		)
		action: actions.ActionBasic
		for action in self.actions:
			cprint(f'\t\t{action.get_config()} = \n'
			       f'\t\t\tconfig = {action.get_config()}',
			       color=C.INFO)
		cprint(
			f'Test result will {"" if saveToTable else "NOT"}be saved on '
			f'[{TestUnitSettings.tablesFolderPath}/{self.name}.table] .',
			color=C.INFO
		)
		
		cprint(f'Have a nice play!', color=C.INFO)
		cprint_separate_line(f'Test info end', color=C.INFO,
		                     fill_len_indent=4, new_line=True)
		
		cprint_separate_line(f'Testing actions', color=C.INFO,
		                     fill_len_indent=4)
		failedTestCase: typ.Optional[TestCase] = None
		for action in self.actions:
			cprint_separate_line(f'For action [{action.get_name()}]',
			                     color=C.INFO, fill_len_indent=8)
			for testCase in self.testCaseSet.caseSet:
				try:
					action(testCase=testCase)
				except Exception as e:
					cprint(f'Exception from action: [{str(e)}]', color=C.ERR)
					failedTestCase = testCase
					break
			if failedTestCase is not None:
				cprint(f'Failed action [{action.get_name()}] on testCase '
				       f'[{failedTestCase}] ',
				       color=C.WA)
				if copyFailedTestCaseToTestSy:
					failedTestCase.copy_to_test_files()
					cprint(f'Failed test SysY file has been copied.',
					       color=C.WA)
			else:
				cprint(f'All testCases are passed in action [{action.get_name()}]! '
				       f'Congratulation!',
				       color=C.AC
				       )
		cprint_separate_line(f'Test end', color=C.INFO)


allTestUnits: typ.Dict[str, TestUnit] = dict(
	map(lambda testUnit: (testUnit.name, testUnit), [
		TestUnit(
			name='empty', testCaseSet=emptyTestCaseSet,
			terminalVerbose=True,
			helpInfo='Just an empty test unit.',
			acts=[
				actions.ActionBasic()
			]
		),
		TestUnit(
			name='test_frontend',
			testCaseSet=basicTestCaseSet,
			terminalVerbose=True,
			helpInfo='Test frontend.',
			acts=[
				actions.JustCompileToLLVMIR(optiLevel=0)
			]
		),
		TestUnit(
			name='test_frontend_with_llc',
			testCaseSet=basicTestCaseSet,
			terminalVerbose=True,
			helpInfo='Test frontend.',
			acts=[
				actions.CompileToLLVMIRAndUseLLC(optiLevel=0)
			]
		),
		TestUnit(
			name='test_frontend_with_llc_on_std_test2020',
			testCaseSet=function2020TestCaseSet,
			terminalVerbose=True,
			helpInfo='Test frontend with llc and on std_test 2020.',
			acts=[
				actions.CompileToLLVMIRAndUseLLC(optiLevel=0)
			]
		),
		TestUnit(
			name='test_frontend_with_llc_on_all_function_test_set',
			testCaseSet=allFunctionTestsCaseSet,
			terminalVerbose=True,
			helpInfo='Test frontend with llc on functional tests.',
			acts=[
				actions.CompileToLLVMIRAndUseLLC(optiLevel=0)
			]
		),
		TestUnit(
			name='test_frontend_on_running',
			testCaseSet=allFunctionTestsCaseSet,
			terminalVerbose=True,
			helpInfo='Test frontend with llc, and run on pi.',
			acts=[
				actions.CompileToLLVMIRAndUseLLCAndRunOnPi(optiLevel=0)
			]
		),
		TestUnit(
			name='test_moe_function_without_float_test',
			testCaseSet=functionWithoutFloat,
			terminalVerbose=True,
			helpInfo='Test MoeCompiler for all function test without float.',
			acts=[
				actions.CompileToASMAndRunOnPi(optiLevel=0)
			]
		),
		TestUnit(
			name='test_moe_function',
			testCaseSet=allFunctionTestsCaseSet,
			terminalVerbose=True,
			helpInfo='Test MoeCompiler for all function tests.',
			acts=[
				actions.CompileToASMAndRunOnPi(optiLevel=0)
			]
		)
	])
)


def list_all_tests():
	cprint_separate_line(f'List of supported tests',
	                     color=C.INFO)
	for testUnit in allTestUnits.values():
		cprint(f'{testUnit.name} : \n'
		       f'\thelp info   = {testUnit.helpInfo}\n'
		       f'\ttestCaseSet = {testUnit.testCaseSet.setName}',
		       color=C.INFO)
		cprint(f'\ttestActions = ', color=C.INFO)
		action: actions.ActionBasic
		for action in testUnit.actions:
			cprint(f'\t\t{action.get_name()} : {action.get_config()}', color=C.INFO)
	cprint_separate_line(f'End of list', color=C.INFO, new_line=True)


if __name__ == '__main__':
	list_all_tests()
