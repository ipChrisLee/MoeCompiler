import json
from pathlib import Path
import typing as typ


class BasicSettings:
	optimizeMoe: bool = True


class TimeoutSettings:
	default: float = 20
	make: float = 60
	cmake: float = 20
	compileMoe: float = 80
	clang: float = 40
	gcc: float = 40
	opt: float = 20
	llc: float = 300
	run: float = 300
	moe: float = 300


class CommandLineSettings:
	verbose: bool = True
	separate_len: int = 81
	separate_char: str = '='


class Commands:
	pass


class TestUnitSettings:
	tablesFolderPath: Path = Path('scripts/result_tables/')
	syPathForFailedTest: Path = Path('testDir/testSyFile/test.sy')
	generalTableFilePath: Path = tablesFolderPath / Path('result.table')
	assert (Path(syPathForFailedTest).exists())
	assert (Path(tablesFolderPath).exists())


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
	gccPerfTimeData: typ.Dict[str, float] = {}
	with open(gccPerfDataFilePath, 'r') as _:
		_data: typ.Dict = json.load(_)
		for _name, _dic in _data.items():
			gccPerfTimeData[_name] = _dic['time_cost']


nullDev: str = '/dev/null'
bufferTextFilePath: str = f'testDir/buffer.txt'
emptyTextFilePath: str = f'testDir/empty.txt'

if not Path(bufferTextFilePath).exists():
	with open(bufferTextFilePath, 'w') as fp:
		pass

if __name__ == '__main__':
	print(JudgmentSettings.gccPerfTimeData)
