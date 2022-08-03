from command_line_tool import run_command
from settings import TimeoutSettings, SysYSettings
import typing as typ


class LLC:
	@staticmethod
	def compile_llvmir(
		llFilePath: str,
		sFilePath: str,
	):
		return run_command(
			[
				'llc', f'{llFilePath}', '-o', f'{sFilePath}'
			], timeout=TimeoutSettings.llc
		)


class Clang:
	@staticmethod
	def compile_to_llvmir(
		syFilePath: str,
		llFilePath: str,
		optiLevel: int = 0
	):
		return run_command(
			[
				'clang', '-Xclang', f'-O{optiLevel}', '-S', '-x', 'c',
				'-emit-llvm',
				'-include', SysYSettings.syLibHeaderPath,
				'-target', 'armv7a-unknown-linux-gnueabihf', '-march=armv7-a',
				'-mfloat-abi=hard',
				'-fno-discard-value-names', '-no-integrated-as',
				f'{syFilePath}', '-o', f'{llFilePath}'
			], timeout=TimeoutSettings.clang
		)
	
	@staticmethod
	def compile_to_ass(
		syFilePath: str,
		sFilePath: str,
		optiLevel: int = 0
	):
		return run_command(
			[
				'clang', '-Xclang', f'-O{optiLevel}', '-S', '-x', 'c',
				'-include', SysYSettings.syLibHeaderPath,
				'-target', 'armv7a-unknown-linux-gnueabihf', '-march=armv7-a',
				'-mfloat-abi=hard',
				'-fno-discard-value-names', '-no-integrated-as',
				f'{syFilePath}', '-o', f'{sFilePath}'
			], timeout=TimeoutSettings.clang
		)


class Opt:
	@staticmethod
	def opt(
		llFilePath: str,
		newLLFilePath: str,
		passes: typ.List[str] = []
	):
		return run_command(
			[
				'opt', '-S', f'{llFilePath}',
				'-o', f'{newLLFilePath}',
				*[f'-{_}' for _ in passes],
				'-debug-pass=Executions',
			]
		)
