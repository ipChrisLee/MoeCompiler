from scripts.pyScript.helper.settings import BasicSettings
from scripts.pyScript.helper.command_line_tool import run_command, TimeoutSettings
import subprocess
from scripts.pyScript.py_tools import classproperty


class Moe:
	_compiled = False
	_optiCompiled = False
	
	@classproperty
	def commandsToCompileMoe(cls):
		return [
			'cmake', '--build', 'cmake-build-release', '--target', 'compiler', '-j6'
		] if BasicSettings.optimizeMoe else [
			'cmake', '--build', 'cmake-build-debug', '--target', 'compiler', '-j6'
		]
	
	@classproperty
	def moePath(cls):
		if BasicSettings.optimizeMoe:
			if not cls._optiCompiled:
				cls._optiCompiled = True
				run_command(
					Moe.commandsToCompileMoe,
					timeout=TimeoutSettings.compileMoe, verbose=True
				).check_returncode()
		else:
			if not cls._compiled:
				cls._compiled = True
				run_command(
					Moe.commandsToCompileMoe,
					timeout=TimeoutSettings.compileMoe, verbose=True
				).check_returncode()
		return \
			'cmake-build-release/compiler' if BasicSettings.optimizeMoe \
				else 'cmake-build-debug/compiler'
	
	@staticmethod
	def compile(
		syFilePath: str,
		msFilePath: str,
		emit_llvm: bool,
		float_dec_format: bool,
		without_any_pass: bool = False,
		emit_dessa: bool = False,
		optiLevel: int = 0,
		timeout: float = 90
	) -> subprocess.CompletedProcess:
		return run_command(
			[
				Moe.moePath,
				syFilePath,
				'-S',
				f'-O{optiLevel}',
				'-o', msFilePath,
				'--emit-llvm' if emit_llvm else '',
				'--emit-dessa' if emit_dessa else '',
				'--float-dec-format' if float_dec_format else '',
				'--without-any-pass' if without_any_pass else '',
			],
			timeout=timeout
		)
