"""
This is for colorful print.
"""
import sys

__BLACK = '\033[0;30m'
__RED = '\033[0;31m'  # use when error happens
__ERR = __RED
__GREEN = '\033[0;32m'  # use when finished
__FIN = __GREEN
__YELLOW = '\033[0;33m'  # use when iteracting with user
__ITERCT = __YELLOW
__BLUE = '\033[0;34m'
__PURPLE = '\033[0;35m'
__CYAN = '\033[0;36m'  # use when working
__WORKING = __CYAN

__BRED = '\033[1;31m'
__BGREEN = '\033[1;32m'
__BYELLOW = '\033[1;33m'
__BBLUE = '\033[1;34m'
__BPURPLE = '\033[1;35m'  # use when warning
__WARNING = __BPURPLE

__UNDERLINE = '\033[4m'

__NOC = '\033[0m'


def cprint(*args, color: str = __ITERCT, sep=' ', end='\n', file=sys.stdout, flush=False):
    print(color, sep=sep, end='', file=file, flush=flush)
    print(*args, sep=sep, end=end, file=file, flush=flush)
    print(__NOC, sep=sep, end='', file=file, flush=flush)


if __name__ == '__main__':
    cprint("123", "321", color=__WARNING)
