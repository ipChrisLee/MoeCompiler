from scripts.pyScript.helper.color import C, cprint, cprint_separate_line
from case_and_case_set import TestCaseSet, TestCase
from scripts.pyScript.helper.settings import TestUnitSettings
import actions
import typing as typ
from case_and_case_set import emptyTestCaseSet, allFunctionTestsCaseSet, \
	functionWithoutFloat, myFuncTestCaseSet
from scripts.pyScript.helper.settings import SysYSettings
from pathlib import Path


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
		copyFailedTestCaseToTestSy: typ.Optional[bool],
		termIfFailed=True
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
			cprint(f'\t\t{action.config} = \n'
			       f'\t\t\tconfig = {action.config}',
			       color=C.INFO)
		cprint(
			f'Test result will {"" if saveToTable else "NOT"}be saved on '
			f'[{TestUnitSettings.tablesFolderPath}/*.table] .',
			color=C.INFO
		)
		if termIfFailed:
			cprint(f'Test unit will be terminated once error occurs.', color=C.INFO)
		
		cprint(f'Have a nice play!', color=C.INFO)
		cprint_separate_line(f'Test info end', color=C.INFO,
		                     fill_len_indent=4, new_line=True)
		
		cprint_separate_line(f'Testing actions', color=C.INFO,
		                     fill_len_indent=4)
		failedTestCase: typ.Optional[TestCase] = None
		for action in self.actions:
			cprint_separate_line(f'For action [{action.name}]',
			                     color=C.INFO, fill_len_indent=8)
			for testCase in self.testCaseSet.caseSet:
				res = action(testCase=testCase)
				if not res.accepted():
					failedTestCase = testCase
			if failedTestCase is not None and termIfFailed:
				cprint(f'Failed action [{action.name}] on testCase '
				       f'[{failedTestCase}] ',
				       color=C.WA)
				if copyFailedTestCaseToTestSy:
					failedTestCase.copy_to_test_files()
					cprint(f'Failed test SysY file has been copied.',
					       color=C.WA)
			else:
				cprint(f'All testCases are passed in action [{action.name}]! '
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
			name='test_moe_function',
			testCaseSet=allFunctionTestsCaseSet,
			terminalVerbose=True,
			helpInfo='Test MoeCompiler for all function tests.',
			acts=[
				actions.CompileToASMAndRunOnPi(optiLevel=0)
			]
		),
		TestUnit(
			name='test_moe_on_my_function',
			testCaseSet=myFuncTestCaseSet,
			terminalVerbose=True,
			helpInfo='Test MoeCompiler for my function test.',
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
			cprint(f'\t\t{action.name} : {action.config}', color=C.INFO)
	cprint_separate_line(f'End of list', color=C.INFO, new_line=True)


if __name__ == '__main__':
	list_all_tests()
