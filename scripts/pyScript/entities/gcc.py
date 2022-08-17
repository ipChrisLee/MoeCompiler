from scripts.pyScript.helper.command_line_tool import run_command
from scripts.pyScript.helper.settings import SysYSettings, TimeoutSettings
import typing as typ


class GCC:
	@staticmethod
	def compile_to_ass(
		syFilePath: str,
		sFilePath: str,
		optiLevel: int = 0
	):
		return run_command(
			[
				'arm-linux-gnueabihf-g++', f'-O{optiLevel}', '-S', '-x', 'c++',
				'-include', SysYSettings.mySyLibHeaderPath,
				'-march=armv7-a+fp', "-fsingle-precision-constant",
				f'{syFilePath}', '-o', f'{sFilePath}'
			], timeout=TimeoutSettings.gcc
		)
