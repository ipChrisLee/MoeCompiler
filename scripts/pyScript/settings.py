from pathlib import Path


class BasicSettings:
	optimizeMoe: bool = False


class TimeoutSettings:
	default: float = 20
	make: float = 60
	cmake: float = 20
	compileMoe: float = 80
	clang: float = 40
	opt: float = 20
	llc: float = 20
	run: float = 20
	moe: float = 90


class CommandLineSettings:
	verbose: bool = True
	separate_len: int = 81
	separate_char: str = '='


class Commands:
	pass


class TestUnitSettings:
	tablesFolderPath: str = 'scripts/pyScript/tables_of_testunit'
	syPathForFailedTest: str = 'testDir/testSyFile/test.sy'


class TestFilesSettings:
	class FilePath:
		_base: str = 'testDir/testSyFile/test'
		testSy: str = f'{_base}.sy'
		testLL: str = f'{_base}.ll'
		testOptLL: str = f'{_base}.opt.ll'
		testMLL: str = f'{_base}.mll'
		testS: str = f'{_base}.s'
		testMS: str = f'{_base}.ms'
		testIn: str = f'{_base}.in'
		testOut: str = f'{_base}.out'
		testRes: str = f'{_base}.res.json'


class SysYSettings:
	syLibHeaderPath: str = 'support/syLibFiles/sylib.h'


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
