from color import C, cprint, cprint_separate_line
from case_and_case_set import TestCaseSet, TestCase
from command_line_tool import run_command, RunningInfo
from settings import TestUnitSettings
import typing as typ


class ActionBasic:
    def __call__(self, testUnit, testCase, **kwargs) -> RunningInfo:
        return RunningInfo(exitCode=0, stdout='', stderr='', timeCost=0)

    def get_config(self) -> str:
        return 'Action with no config.'


class TestUnit:
    def __init__(
            self,
            name: str,
            testCaseSet: TestCaseSet,
            actions: typ.Dict[str, ActionBasic],
            terminalVerbose: bool = True,
            helpInfo: str = 'No help info.'
    ):
        self.name = name
        self.testCaseSet = testCaseSet
        self.terminalVerbose = terminalVerbose
        self.helpInfo = helpInfo
        self.actions = actions

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
        actionName: str
        action: ActionBasic
        for [actionName, action] in self.actions.items():
            cprint(f'\t\t{actionName} = \n'
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
        for [actionName, action] in self.actions.items():
            cprint_separate_line(f'For action [{actionName}]',
                                 color=C.INFO, fill_len_indent=8)
            for testCase in self.testCaseSet.caseSet:
                runningInfo: RunningInfo = action(testUnit=self,
                                                  testCase=testCase)
                if runningInfo.failed():
                    failedTestCase = testCase
                    break
            if failedTestCase is not None:
                cprint(f'Failed action [{actionName}] on testCase '
                       f'[{failedTestCase}] ',
                       color=C.WA)
                if copyFailedTestCaseToTestSy:
                    run_command('cp', failedTestCase.syFilePath,
                                TestUnitSettings.syPathForFailedTest)
                    cprint(f'Failed test SysY file has been copied.',
                           color=C.WA)
            else:
                cprint(f'All testCases are passed in action [{actionName}]! '
                       f'Congratulation!',
                       color=C.AC
                       )
        cprint_separate_line(f'Test end', color=C.INFO)


def list_all_tests(allTestUnits: typ.Dict[str, TestUnit]):
    cprint_separate_line(f'List of supported tests',
                         color=C.INFO)
    for testUnit in allTestUnits.values():
        cprint(f'{testUnit.name} : \n'
               f'\thelp info   = {testUnit.helpInfo}\n'
               f'\ttestCaseSet = {testUnit.testCaseSet.setName}',
               color=C.INFO)
        cprint(f'\ttestActions = ', color=C.INFO)
        actionName: str
        action: ActionBasic
        for actionName, action in testUnit.actions.items():
            cprint(f'\t\t{actionName} : {action.get_config()}', color=C.INFO)
    cprint_separate_line(f'End of list', color=C.INFO, new_line=True)
