"""
This is for colorful print.
"""
import sys
from enum import Enum
from scripts.pyScript.helper.settings import CommandLineSettings


class C(Enum):
	BLACK = '\033[0;30m'
	RED = '\033[0;31m'  # use when error happens
	ERR = RED
	GREEN = '\033[0;32m'  # use when finished
	FIN = GREEN
	YELLOW = '\033[0;33m'  # use when interacting with user
	BLUE = '\033[0;34m'
	INTERACT = BLUE
	PURPLE = '\033[0;35m'
	CYAN = '\033[0;36m'  # use when working
	WORKING = CYAN
	INFO = CYAN
	
	BRED = '\033[1;31m'
	BGREEN = '\033[1;32m'
	BYELLOW = '\033[1;33m'
	TERMINAL = BYELLOW
	BBLUE = '\033[1;34m'
	BPURPLE = '\033[1;35m'  # use when warning
	WARNING = BPURPLE
	
	UNDERLINE = '\033[4m'
	
	NOC = '\033[0m'
	
	AC = str(BGREEN) + str(UNDERLINE)
	WA = str(BRED) + str(UNDERLINE)


def cprint(*args, color: C = C.INTERACT, sep=' ', end='\n', file=sys.stdout,
           flush=False
           ):
	print(color.value, sep=sep, end='', file=file, flush=flush)
	print(*args, sep=sep, end=end, file=file, flush=flush)
	print(C.NOC.value, sep=sep, end='', file=file, flush=flush)


def cprint_separate_line(content: str,
                         fill_char=CommandLineSettings.separate_char,
                         fill_len=CommandLineSettings.separate_len,
                         fill_len_indent: int = 0,
                         color: C = C.INTERACT,
                         new_line: bool = False
                         ):
	fill_center_len = fill_len - 2 * fill_len_indent
	assert (fill_len >= 0)
	cprint(content.center(fill_center_len, fill_char).center(fill_len, ' '),
	       color=color)
	if new_line:
		cprint()


if __name__ == '__main__':
	cprint("123", "321", color=C.AC)
