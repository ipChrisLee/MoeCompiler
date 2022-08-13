from os import walk
from pathlib import Path
from enum import Enum
import typing as typ
from scripts.pyScript.helper.settings import nullDev, emptyTextFilePath, \
	TestFilesSettings
from scripts.pyScript.entities.Bash import Bash


class TestType(Enum):
	Basic = "basic"
	Function = "functional"
	Performance = "performance"


class TestCase:
	info: typ.Dict[str, str] = dict()
	
	def __init__(
		self,
		testName: str,
		syFilePath: str, inFilePath: str, outFilePath: str,
		year: int, testType: TestType
	):
		self._testName: str = testName
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
		Path(self.msFilePath).touch()
	
	def __str__(self):
		return str(self.year) + "::" + self.testType.name + "::" + self._testName
	
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


myFuncTestCaseSet = TestCaseSet(
	load_testcases_under_folder_path(
		'testcases/my_function_test', 2022, TestType.Function
	), "m2022"
)

allFunctionTestsCaseSet = TestCaseSet(
	set.union(
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2022", 2022, TestType.Function
		),
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2022_hidden", 2022,
			TestType.Function
		)
	), "function"
)

allPerformanceTestsCaseSet = TestCaseSet(
	load_testcases_under_folder_path(
		'testcases/SysYTestCase/performance_test2022',
		2022, TestType.Performance
	), 'performance'
)

functionWithoutFloat = TestCaseSet(
	set.union(
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2021", 2021, TestType.Function
		),
		load_testcases_under_folder_path(
			"testcases/SysYTestCase/function_test2020", 2020, TestType.Function
		),
	), 'withoutFloat'
)

emptyTestCaseSet = TestCaseSet(set(), 'empty')

if __name__ == '__main__':
	pass
