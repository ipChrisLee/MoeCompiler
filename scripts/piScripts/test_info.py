from enum import Enum
import typing as typ
import math


class TestStatus(Enum):
	AC = 'AC'
	WA = 'WA'
	TLE = 'TLE'
	CE = 'CE'
	RE = 'RE'
	
	@staticmethod
	def from_str_to_TestStatus(s: str):
		if s == 'AC':
			return TestStatus.AC
		elif s == 'WA':
			return TestStatus.WA
		elif s == 'TLE':
			return TestStatus.TLE
		elif s == 'CE':
			return TestStatus.CE
		elif s == 'RE':
			return TestStatus.RE
		else:
			return None


class RunningInfo:
	def __init__(
		self,
		exit_code: int,
		info: str,
		test_status: TestStatus,
		message: typ.Optional[str] = None,
		out: typ.Optional[str] = None,
		stderr: typ.Optional[str] = None,
		time_now: typ.Optional[str] = None,
		time_cost: float = math.nan
	):
		"""
			exit_code:
				Status when leaving test.py .
			info:
				Information of exit_status test.py .
			message:
				[optional]
				Other messages except info.
			out:
				[optional]
				Stdout of executable main file and its return code.
			stderr:
				[optional]
				Stderr of executable main file.
			test_status:
				Value:
					{CE} if something happened before running main.
					{AC,WA,TLE} otherwise.
				Status of test.
			time_now:
				Time when leaving test.py .
			time_cost:
				[optional]
				Time cost get from stderr.
		"""
		self.exit_code = exit_code
		self.info = info
		self.test_status = test_status
		self.message = message
		self.out = out
		self.stderr = stderr
		self.time_now = time_now
		if not isinstance(time_cost, float):
			time_cost = math.nan
		self.time_cost = time_cost
	
	def to_dict(self):
		res = dict()
		res['exit_code'] = self.exit_code
		res['info'] = self.info
		res['test_status'] = self.test_status.name
		if self.message is not None:
			res['message'] = self.message
		if self.out is not None:
			res['out'] = self.out
		if self.stderr is not None:
			res['stderr'] = self.stderr
		if self.time_now is not None:
			res['time_now'] = self.time_now
		res['time_cost'] = self.time_cost
		return res
	
	@staticmethod
	def new_from_dict(dic: dict):
		if isinstance(dic['test_status'], str):
			dic['test_status'] = TestStatus.from_str_to_TestStatus(
				dic['test_status']
			)
		return RunningInfo(**dic)
	
	def accepted(self):
		return self.test_status == TestStatus.AC
	
	def to_list_info(self, testName: str, stdTimeCost: float):
		return [testName, self.test_status.name, self.time_cost,
		        self.info, self.stderr, self.time_cost / stdTimeCost]
	
	header = ['test_name', 'status', 'time_cost', 'info', 'stderr', 'score']
