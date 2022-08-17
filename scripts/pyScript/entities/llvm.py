from scripts.pyScript.helper.command_line_tool import run_command
from scripts.pyScript.helper.settings import TimeoutSettings, SysYSettings


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
		llFilePath: str
	):
		return run_command(
			[
				'clang', '-Xclang', '-emit-llvm', '-S', '-x', 'c',
				'-disable-O0-optnone',
				'-include', SysYSettings.syLibHeaderPath,
				'-target', 'armv7a-unknown-linux-gnueabihf', '-march=armv7-a',
				'-mfloat-abi=hard',
				'-fno-discard-value-names',
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
				'clang', '-Xclang', '-S', '-x', 'c',
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
		passes=None
	):
		if passes is None:
			passes = ['mem2reg']
		return run_command(
			[
				'opt', '-S', f'{llFilePath}',
				'-o', f'{newLLFilePath}',
				*[f'-{_}' for _ in passes],
				'-debug-pass=Executions',
			]
		)
