from os import walk
from pathlib import Path
from enum import Enum
import typing as typ
from settings import nullDev, emptyTextFilePath, TestFilesSettings
from Bash import Bash


class TestType(Enum):
	Basic = "basic"
	Function = "functional"
	Performance = "performance"


class TestCase:
	info: typ.Dict[str, str] = dict()
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
		if not Path(inFilePath).exists():
			inFilePath = emptyTextFilePath
		self.inFilePath: str = inFilePath
		if not Path(outFilePath).exists():
			outFilePath = emptyTextFilePath
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
	
	def copy_to_test_files(self):
		Bash.file_copy(
			srcFilePath=self.syFilePath,
			dstFilePath=TestFilesSettings.FilePath.testSy
		)
		Bash.file_copy(
			srcFilePath=self.inFilePath,
			dstFilePath=TestFilesSettings.FilePath.testIn
		)
		Bash.file_copy(
			srcFilePath=self.outFilePath,
			dstFilePath=TestFilesSettings.FilePath.testOut
		)


class TestCaseSet:
	def __init__(self, caseSet: typ.Set[TestCase], setName: str):
		self.caseSet = caseSet
		self.setName = setName


def load_testcases_under_folder_path(
	fPath: str, year: int, testType: TestType
) -> set:
	res = set()
	for (path, folders, files) in walk(fPath):
		for file in files:
			if not Path(file).suffix == '.sy':
				continue
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


allFunctionTestsCaseSet = TestCaseSet(
	set.union(
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2020", 2020, TestType.Function
		),
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2021", 2021, TestType.Function
		),
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2022", 2022, TestType.Function
		)
	), "function"
)

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
