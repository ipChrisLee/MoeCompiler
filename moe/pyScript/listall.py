import __bcolor
from __bcolor import cprint
import os
from os import walk
import glob
from pathlib import Path

ignoreFiles = ['__bcolor', 'pytest']

if __name__ == '__main__':
    f = []
    pysPath = os.getcwd() + '/pyScript/*.py'
    allPyFile = [Path(_).stem for _ in glob.glob(pysPath)]
    for _ in ignoreFiles:
        allPyFile.remove(_)
    cprint("Support python scripts : ")
    cprint("   ", *allPyFile)
