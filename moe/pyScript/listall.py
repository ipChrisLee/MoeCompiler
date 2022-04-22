import __bcolor
from __bcolor import cprint
import os
from os import walk
import glob
from pathlib import Path

if __name__ == '__main__':
    f = []
    pysPath = os.getcwd() + '/pyScript/*.py'
    allPyFile = [Path(_).stem for _ in glob.glob(pysPath)]
    allPyFile.remove('__bcolor')
    cprint("Support python scripts : ")
    cprint("   ", *allPyFile)
