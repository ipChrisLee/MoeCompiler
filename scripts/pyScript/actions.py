from scripts.pyScript.helper.settings import bufferTextFilePath
from scripts.pyScript.entities.Moe import Moe
from scripts.pyScript.entities.llvm import LLC
from scripts.pyScript.entities.Pi import Pi
from case_and_case_set import TestCase
from scripts.piScripts.test_info import TestStatus, RunningInfo


class ActionBasic:
	def __call__(self, testCase, **kwargs) -> RunningInfo:
		return RunningInfo(exit_code=0, info='', test_status=TestStatus.AC)
	
	@property
	def config(self) -> str:
		return 'Action with no config.'
	
	@property
	def name(self) -> str:
		return 'BasicAction'


class JustCompileToLLVMIR(ActionBasic):
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	@property
	def config(self) -> str:
		return f'Just compile sy file to llvm-ir, with optiLevel=' \
		       f'{self.optiLevel}.'
	
	@property
	def name(self) -> str:
		return f'Just_O{self.optiLevel}Compile_To_LLVMIR'
	
	def __call__(self, testCase: TestCase, **kwargs):
		res = Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=testCase.msFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel
		)
		if res.returncode != 0:
			return RunningInfo(
				exit_code=res.returncode,
				info='Error when use moe compile SysY file.',
				test_status=TestStatus.CE,
				stderr=res.stderr
			)
		else:
			return RunningInfo(
				exit_code=res.returncode,
				info='',
				test_status=TestStatus.AC,
				stderr=res.stderr
			)


class CompileToLLVMIRAndUseLLC(ActionBasic):
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	@property
	def config(self) -> str:
		return f'Compile sy file to llvm-ir use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}. Use clang compile llvm-ir ' \
		       f'then.'
	
	@property
	def name(self) -> str:
		return f'O{self.optiLevel}Compile_To_LLVMIR_And_Use_LLC'
	
	def __call__(self, testCase: TestCase, **kwargs):
		res = Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=bufferTextFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel
		)
		if res.returncode != 0:
			return RunningInfo(
				exit_code=res.returncode,
				info='Error when use moe compile SysY file.',
				test_status=TestStatus.CE,
				stderr=res.stderr
			)
		res = LLC.compile_llvmir(
			llFilePath=bufferTextFilePath,
			sFilePath=testCase.msFilePath
		)
		if res.returncode != 0:
			return RunningInfo(
				exit_code=res.returncode,
				info='LLC failed.',
				stderr=res.stderr,
				test_status=TestStatus.CE
			)
		else:
			return RunningInfo(
				exit_code=res.returncode,
				info='LLC.',
				stderr=res.stderr,
				test_status=TestStatus.AC
			)


class CompileToLLVMIRAndUseLLCAndRunOnPi(ActionBasic):
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	@property
	def config(self) -> str:
		return f'Compile sy file to llvm-ir use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}. Use llc compile llvm-ir.' \
		       f'Run on pi.'
	
	def __call__(self, testCase, **kwargs):
		res = Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=bufferTextFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel
		)
		if res.returncode != 0:
			return RunningInfo(
				exit_code=res.returncode,
				test_status=TestStatus.CE,
				stderr=res.stderr,
				info='Fail to compile SysY with Moe.'
			)
		res = LLC.compile_llvmir(
			llFilePath=bufferTextFilePath,
			sFilePath=testCase.msFilePath
		)
		if res.returncode != 0:
			return RunningInfo(
				exit_code=res.returncode,
				test_status=TestStatus.CE,
				stderr=res.stderr,
				info='LLC Failed.'
			)
		res = Pi.run_tester(
			sFilePath=testCase.msFilePath,
			inFilePath=testCase.inFilePath,
			outFilePath=testCase.outFilePath,
			resFilePath=bufferTextFilePath
		)
		return res
	
	@property
	def name(self) -> str:
		return f'O{self.optiLevel}Compile_To_LLVMIR_And_Use_LLC'


class CompileToASMAndRunOnPi(ActionBasic):
	@property
	def config(self) -> str:
		return f'Compile sy file to asm use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}.' \
		       f'Then run on pi.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testCase, **kwargs):
		res = Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=testCase.msFilePath,
			emit_llvm=False,
			float_dec_format=False,
			optiLevel=self.optiLevel
		)
		if res.returncode != 0:
			return RunningInfo(
				exit_code=res.returncode,
				stderr=res.stderr,
				test_status=TestStatus.CE,
				info='Failed to compile SysY with Moe.'
			)
		res = Pi.run_tester(
			sFilePath=testCase.msFilePath,
			inFilePath=testCase.inFilePath,
			outFilePath=testCase.outFilePath,
			resFilePath=bufferTextFilePath
		)
		return res
	
	@property
	def name(self) -> str:
		return f'O{self.optiLevel}Compile_To_ASM_And_Run_On_Pi'
