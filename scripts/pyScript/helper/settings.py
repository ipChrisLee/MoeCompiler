import json
from pathlib import Path


class BasicSettings:
	optimizeMoe: bool = False


class TimeoutSettings:
	default: float = 20
	make: float = 60
	cmake: float = 20
	compileMoe: float = 80
	clang: float = 40
	gcc: float = 40
	opt: float = 20
	llc: float = 20
	run: float = 300
	moe: float = 90


class CommandLineSettings:
	verbose: bool = True
	separate_len: int = 81
	separate_char: str = '='


class Commands:
	pass


class TestUnitSettings:
	tablesFolderPath: Path = Path('scripts/result_tables/')
	syPathForFailedTest: Path = Path('testDir/testSyFile/test.sy')


class TestFilesSettings:
	class FilePath:
		_base: str = 'testDir/testSyFile/test'
		testSy: str = f'{_base}.sy'
		testLL: str = f'{_base}.ll'
		testMLL: str = f'{_base}.mll'
		testS: str = f'{_base}.s'
		testMS: str = f'{_base}.ms'
		testIn: str = f'{_base}.in'
		testOut: str = f'{_base}.out'
		testRes: str = f'{_base}.res.json'
		testOptLL: str = f'{_base}.opt.ll'


class SysYSettings:
	syLibHeaderPath: str = 'support/syLibFiles/sylib.h'
	mySyLibHeaderPath: str = 'support/syLibFiles/mySyLib.h'


class JudgmentSettings:
	gccPerfDataFilePath: Path = Path('scripts/result_tables/gcc_perf.json')
	assert gccPerfDataFilePath.exists()
	gccPerfTableFilePath: Path = Path('scripts/result_tables/gcc_perf.table')
	assert gccPerfTableFilePath.exists()


assert (Path(TestUnitSettings.syPathForFailedTest).exists())
assert (Path(TestUnitSettings.tablesFolderPath).exists())

nullDev: str = '/dev/null'
bufferTextFilePath: str = f'testDir/buffer.txt'
emptyTextFilePath: str = f'testDir/empty.txt'

if not Path(bufferTextFilePath).exists():
	with open(bufferTextFilePath, 'w') as fp:
		pass

if __name__ == '__main__':
	pass
