from color import cprint, C
from os import walk
from pathlib import Path
from enum import Enum
import typing
from settings import nullDev
from command_line_tool import run_command


class TestType(Enum):
    Basic = "basic"
    Function = "functional"
    Performance = "performance"


class TestCase:
    """
    What to return after action? : (exit_code,syFilePath)
    """

    def __init__(
            self,
            testName: str,
            syFilePath: str, inFilePath: str, outFilePath: str,
            year: int, testType: TestType
    ):
        self.testName = testName
        self.syFilePath = syFilePath
        self.inFilePath = inFilePath
        self.outFilePath = outFilePath
        self.year = year
        self.testType = testType
        self.msFilePath = Path(syFilePath).with_suffix(".ms")

    def __str__(self):
        return str(self.year) + "::" + self.testType.name + "::" + self.testName

    def __hash__(self):
        return hash(self.syFilePath)

    def just_compile_without_opti(self, *args, moecompilerPath):
        return run_command(
            moecompilerPath, self.syFilePath, "-S", "-o", self.msFilePath, *args
        )[0], self.syFilePath


class TestCaseSet:
    def __init__(self, caseSet: typing.Set[TestCase], setName: str):
        self.caseSet = caseSet
        self.setName = setName


def load_testcases_under_folder_path(fPath: str, year: int, testType: TestType):
    res = set()
    for (path, folders, files) in walk(fPath):
        for file in files:
            testName = Path(file).stem
            thisCase = TestCase(
                testName,
                path + "/" + testName + ".sy",
                path + "/" + testName + ".in",
                path + "/" + testName + ".out",
                year, testType)
            if not Path(thisCase.inFilePath).exists():
                thisCase.inFilePath = nullDev
            res.add(thisCase)
    return res


function2020TestCaseSet = TestCaseSet(
    load_testcases_under_folder_path(
        "testcases/SysYTestCase/function_test2020", 2020, TestType.Function
    ), "f2020"
)

function2021TestCaseSet = TestCaseSet(
    load_testcases_under_folder_path(
        "testcases/SysYTestCase/function_test2021", 2021, TestType.Function
    ), "f2021"
)

performance2021TestCaseSet = TestCaseSet(
    set.union(
        load_testcases_under_folder_path(
            "testcases/SysYTestCase/performance_test2021-private", 2021,
            TestType.Performance
        ),
        load_testcases_under_folder_path(
            "testcases/SysYTestCase/performance_test2021-public", 2021,
            TestType.Performance
        )
    ), "p2021"
)

allTestCaseSet = TestCaseSet(
    set.union(
        function2021TestCaseSet.caseSet,
        function2020TestCaseSet.caseSet,
        performance2021TestCaseSet.caseSet
    ), "all"
)

basicTestCaseSet = TestCaseSet(
    load_testcases_under_folder_path(
        'testcases/BasicTestCase', 2022, TestType.Basic
    ), "b2022"
)

if __name__ == '__main__':
    pass
