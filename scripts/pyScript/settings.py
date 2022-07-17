from pathlib import Path


class BasicSettings:
    optimizeMoe: bool = False


class TimeoutSettings:
    default: float = 20
    make: float = 60
    cmake: float = 20
    compileMoe: float = 80


class CommandLineSettings:
    verbose: bool = True
    separate_len: int = 81
    separate_char: str = '='


class Commands:
    compileMoe: str = [
        "cmake", "--build", "cmake-build-release", "--target", "compiler", "-j1"
    ] if BasicSettings.optimizeMoe else [
        "cmake", "--build", "cmake-build-debug", "--target", "compiler", "-j1"
    ]


class MoeCompilerSettings:
    moePath: str = 'cmake-build-release/compiler' if \
        BasicSettings.optimizeMoe else 'cmake-build-debug/compiler'


class TestUnitSettings:
    tablesFolderPath: str = 'scripts/pyScript/tables_of_testunit'
    syPathForFailedTest: str = 'testDir/testSyFile/test.sy'


assert (Path(TestUnitSettings.syPathForFailedTest).exists())
assert (Path(TestUnitSettings.tablesFolderPath).exists())

nullDev: str = '/dev/null'
bufferTextFilePath: str = f'testDir/buffer.txt'

if not Path(bufferTextFilePath).exists():
    with open(bufferTextFilePath, 'w') as fp:
        pass

if __name__ == '__main__':
    pass
