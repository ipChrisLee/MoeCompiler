import subprocess
import time
from color import C, cprint
from settings import TimeoutSettings, CommandLineSettings, \
	MoeCompilerSettings, Commands


class RunningInfo:
	def __init__(
			self,
			exitCode: int,
			stdout: str,
			stderr: str,
			timeCost: float
	):
		self.exitCode = exitCode
		self.stdout = stdout
		self.stderr = stderr
		self.timeCost = timeCost

	def failed(self) -> bool:
		return self.exitCode != 0


def run_command(
		*args,
		inputStr: str = "",
		timeout: float = TimeoutSettings.default,
		cwd: str = './',
		verbose: bool = None
) -> RunningInfo:
	argsWithoutEmptyStr = list()
	for arg in args:
		if len(arg) is not 0:
			argsWithoutEmptyStr.append(arg)
	if verbose is None:
		verbose = CommandLineSettings.verbose
	if verbose:
		cprint(f'[Terminal (timeout = {timeout:5.2f}s) ] :', *argsWithoutEmptyStr,
		       color=C.TERMINAL)
	startTime = time.time()
	p = subprocess.run(argsWithoutEmptyStr, input=inputStr, stdout=subprocess.PIPE,
	                   stderr=subprocess.PIPE, timeout=timeout, cwd=cwd)
	endTime = time.time()
	costTime = endTime - startTime  # may be inaccurate.
	stdout = p.stdout.decode('utf-8')
	stderr = p.stderr.decode('utf-8')
	exitCode = p.returncode & 0xff
	return RunningInfo(exitCode=exitCode, stdout=stdout, stderr=stderr,
	                   timeCost=float(costTime))


_ = run_command(
	*Commands.compileMoe,
	timeout=TimeoutSettings.compileMoe, verbose=True
)
if _.exitCode != 0:
	cprint(f'Compilation failed! stderr=[{_.stderr}]', color=C.ERR)
	exit(-1)


def use_moe_compile_sy(
		syFilePath: str,
		msFilePath: str,
		emit_llvm: bool,
		float_dec_format: bool,
		optiLevel: int = 0,
		terminalVerbose: bool = CommandLineSettings.verbose
) -> RunningInfo:
	return run_command(
		MoeCompilerSettings.moePath,
		syFilePath,
		'-S',
		f'-O{optiLevel}',
		'-o', msFilePath,
		'--emit-llvm' if emit_llvm else '',
		'--float-dec-format' if float_dec_format else '',
		verbose=terminalVerbose,
	)


if __name__ == '__main__':
	print(run_command('sleep', '1').timeCost)
