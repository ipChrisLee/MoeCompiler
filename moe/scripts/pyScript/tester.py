from color import C, cprint
from case_and_case_set import TestCaseSet, basicTestCaseSet
from command_line_tool import build_compiler, clear_build
from settings import moecompilerPath
import argparse


class Tester:
    def __init__(self, name: str, action, helpInfo: str = 'No help.'):
        self.name = name
        self.action = action
        self.helpInfo = helpInfo

    def __hash__(self):
        return hash(self.name)


def just_compile_to_llvmir_on_testcase_sy(testCaseSet: TestCaseSet):
    res = (0, '')
    for testCase in testCaseSet.caseSet:
        res = testCase.just_compile_without_opti(
            '--emit-llvm', moecompilerPath=moecompilerPath
        )
        if res[0] != 0:
            break
    return res


testers = dict()
for item in [
    Tester(
        'compile_on_basic_testcase',
        lambda: just_compile_to_llvmir_on_testcase_sy(basicTestCaseSet),
        'Compile files to llvm-ir.'
    ),
]:
    testers[item.name] = item


def list_all_tests():
    cprint('List of supported tests:')
    for tester in testers.values():
        cprint(f'Name : {tester.name} , helpInfo : "{tester.helpInfo}".')


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
argParser.add_argument(
    '--build_if_not_exists',
    help='Build moecompiler if moecompiler does NOT exist.',
    action='store_true'
)
argParser.add_argument(
    '--build_even_if_already_exists',
    help='Build moecompiler even if moecompiler already exists.',
    action='store_true'
)
argParser.add_argument(
    '--rm_build',
    help='`rm -rf build` after test.',
    action='store_true'
)

if __name__ == '__main__':
    args = argParser.parse_args()
    if args.build_even_if_already_exists:
        build_compiler()
    if args.build_if_not_exists:
        build_compiler(True)
    if args.list:
        list_all_tests()
    if args.testName:
        if args.testName not in testers.keys():
            cprint(f'Unknown/Unsupported test: {args.testName}!', color=C.ERR)
        else:
            exit_code, syFilePath = testers[args.testName].action()
            if exit_code != 0:
                cprint(f'Test stopped on file[{syFilePath}], exit_code = '
                       f'{exit_code}', color=C.ERR)
            else:
                cprint(f'Test [{args.testName}] passed!', color=C.FIN)
    if args.rm_build:
        clear_build()
