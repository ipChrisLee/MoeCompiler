import subprocess
from scripts.pyScript.helper.color import C, cprint
from scripts.pyScript.helper.settings import TimeoutSettings, CommandLineSettings
import typing as typ


def run_command(
	args: typ.List[str],
	inputStr: str = "",
	timeout: float = TimeoutSettings.default,
	cwd: str = './',
	verbose: bool = None
) -> subprocess.CompletedProcess:
	argsWithoutEmptyStr = list()
	for arg in args:
		if len(arg) is not 0:
			argsWithoutEmptyStr.append(arg)
	if verbose is None:
		verbose = CommandLineSettings.verbose
	if verbose:
		cprint(
			f'[Terminal (timeout = {timeout:5.2f}s) ] :', *argsWithoutEmptyStr,
			color=C.TERMINAL
		)
	return subprocess.run(
		argsWithoutEmptyStr, input=inputStr, stdout=subprocess.PIPE,
		stderr=subprocess.PIPE, timeout=timeout, cwd=cwd, text=str
	)


if __name__ == '__main__':
	pass
