from os import walk
from pathlib import Path
from enum import Enum
import typing
from settings import nullDev


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
        self.testName: str = testName
        self.syFilePath: str = syFilePath
        self.inFilePath: str = inFilePath
        self.outFilePath: str = outFilePath
        self.year: int = year
        self.testType: TestType = testType
        self.msFilePath: str = str(Path(syFilePath).with_suffix(".ms"))

    def __str__(self):
        return str(self.year) + "::" + self.testType.name + "::" + self.testName

    def __hash__(self):
        return hash(self.syFilePath)

    def __eq__(self, other):
        if isinstance(other, TestCase):
            return hash(self) == hash(other)
        return False


class TestCaseSet:
    def __init__(self, caseSet: typing.Set[TestCase], setName: str):
        self.caseSet = caseSet
        self.setName = setName


def load_testcases_under_folder_path(
        fPath: str, year: int, testType: TestType) -> set:
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

emptyTestCaseSet = TestCaseSet(set(), 'empty')

if __name__ == '__main__':
    for testCase in basicTestCaseSet.caseSet:
        print(hash(testCase))
        print(hash(testCase.syFilePath))
