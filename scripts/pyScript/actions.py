from settings import bufferTextFilePath
from Moe import Moe
from llvm import LLC
from Pi import Pi
from case_and_case_set import TestCase, TestType


class ActionBasic:
	def __call__(self, testCase, **kwargs):
		pass
	
	def get_config(self) -> str:
		return 'Action with no config.'
	
	def get_name(self) -> str:
		return 'Basic_Action'


class JustCompileToLLVMIR(ActionBasic):
	def get_config(self) -> str:
		return f'Just compile sy file to llvm-ir, with optiLevel=' \
		       f'{self.optiLevel}.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testCase: TestCase, **kwargs):
		Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=testCase.msFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel
		).check_returncode()
	
	def get_name(self) -> str:
		return f'Just_O{self.optiLevel}Compile_To_LLVMIR'


class CompileToLLVMIRAndUseLLC(ActionBasic):
	def get_config(self) -> str:
		return f'Compile sy file to llvm-ir use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}. Use clang compile llvm-ir ' \
		       f'then.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testCase: TestCase, **kwargs):
		Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=bufferTextFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel
		).check_returncode()
		LLC.compile_llvmir(
			llFilePath=bufferTextFilePath,
			sFilePath=testCase.msFilePath
		).check_returncode()
	
	def get_name(self) -> str:
		return f'O{self.optiLevel}Compile_To_LLVMIR_And_Use_LLC'


class CompileToLLVMIRAndUseLLCAndRunOnPi(ActionBasic):
	def get_config(self) -> str:
		return f'Compile sy file to llvm-ir use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}. Use llc compile llvm-ir.' \
		       f'Run on pi.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testCase, **kwargs):
		Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=bufferTextFilePath,
			emit_llvm=True,
			float_dec_format=False,
			optiLevel=self.optiLevel
		).check_returncode()
		LLC.compile_llvmir(
			llFilePath=bufferTextFilePath,
			sFilePath=testCase.msFilePath
		).check_returncode()
		res = Pi.run_tester(
			sFilePath=testCase.msFilePath,
			inFilePath=testCase.inFilePath,
			outFilePath=testCase.outFilePath,
			resFilePath=bufferTextFilePath
		)
		if res['exit_status'] != 0:
			raise Exception()
	
	def get_name(self) -> str:
		return f'O{self.optiLevel}Compile_To_LLVMIR_And_Use_LLC'


class CompileToASMAndRunOnPi(ActionBasic):
	def get_config(self) -> str:
		return f'Compile sy file to asm use moe-compiler ' \
		       f'with optiLevel={self.optiLevel}.' \
		       f'Then run on pi.'
	
	def __init__(self, optiLevel: int):
		self.optiLevel = optiLevel
	
	def __call__(self, testCase, **kwargs):
		Moe.compile(
			syFilePath=testCase.syFilePath,
			msFilePath=testCase.msFilePath,
			emit_llvm=False,
			float_dec_format=False,
			optiLevel=self.optiLevel
		).check_returncode()
		res = Pi.run_tester(
			sFilePath=testCase.msFilePath,
			inFilePath=testCase.inFilePath,
			outFilePath=testCase.outFilePath,
			resFilePath=bufferTextFilePath
		)
		if res['exit_status'] != 0:
			raise Exception()
	
	def get_name(self) -> str:
		return f'O{self.optiLevel}Compile_To_ASM_And_Run_On_Pi'
