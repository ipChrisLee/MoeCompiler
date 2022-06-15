import subprocess
from settings import TimeoutSettings
from pathlib import Path
from color import C, cprint


def run_command(*args, inputStr: str = "",
                timeout: float = TimeoutSettings.default, cwd='./'):
    p = subprocess.run(args, input=inputStr, stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=timeout, cwd=cwd)
    stdout = p.stdout.decode('utf-8')
    stderr = p.stderr.decode('utf-8')
    exitCode = p.returncode & 0xff
    return exitCode, stdout, stderr


moecompilerPath = "build/compiler"


def build_compiler(doNotBuildIfExists: bool = False):
    if doNotBuildIfExists and Path(moecompilerPath).exists():
        return
    run_command('mkdir', '-p', 'build')
    exit_code, _, stderr = run_command('cmake', '..', cwd='build/',
                                       timeout=TimeoutSettings.cmake)
    if exit_code != 0:
        cprint(f'cmake failed. info:{stderr}', color=C.ERR)
        exit(-1)
    exit_code, _, stderr = run_command('make', 'compiler', '-j', '6',
                                       cwd='build/',
                                       timeout=TimeoutSettings.make)
    if exit_code != 0:
        cprint(f'Compilation failed! info:{stderr}', color=C.ERR)
        exit(-1)


def clear_build():
    run_command('rm', '-rf', 'build')


if __name__ == '__main__':
    run_command(moecompilerPath, 'testDir/test.sy', '-S', '-o',
                'testDir/test.ms', timeout=1.5)
