from test_unit import list_all_tests, allTestUnits
import argparse
from scripts.pyScript.helper.settings import TestUnitSettings
from scripts.pyScript.helper.color import cprint, C

argParser = argparse.ArgumentParser(
	description='Tester arguments.'
)
argParser.add_argument(
	'-t', '--test',
	help='Specify a test to run. [-l]/[--list] shows all supported tests.',
	dest='testName'
)
argParser.add_argument(
	'-l', '--list',
	help='List all available tests.',
	action='store_true'
)
# argParser.add_argument(
#     '--write_error_file_to_testDir_test_sy',
#     help='If error occurs, write error sy file to testDir/test.sy',
#     action='store_true'
# )
argParser.add_argument(
	'--save_result_to_table',
	help='Save test result to table.',
	action='store_true'
)
argParser.add_argument(
	'--copy_failed_test_sy',
	help=f'Save failed test sy file to {TestUnitSettings.syPathForFailedTest}.',
	action='store_true'
)
argParser.add_argument(
	'--term_if_failed',
	help=f'Once a testcase failed, term tester.',
	action='store_true'
)
argParser.add_argument(
	'--verbose_of_terminal',
	help=f'Show every command tester runs.',
	action='store_true'
)

if __name__ == '__main__':
	clArgs = argParser.parse_args()
	if clArgs.list:
		list_all_tests()
	if clArgs.testName:
		if clArgs.testName not in allTestUnits.keys():
			cprint(f'Unknown/Unsupported test: {clArgs.testName}!', color=C.ERR)
		else:
			allTestUnits[clArgs.testName].do_this_test(
				saveToTable=clArgs.save_result_to_table,
				copyFailedTestCaseToTestSy=clArgs.copy_failed_test_sy,
				termIfFailed=clArgs.term_if_failed
			)
	cprint(f'Tester Finished!', color=C.FIN)
