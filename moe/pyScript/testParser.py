import __bcolor
from __bcolor import cprint
import subprocess
from os import walk
from pathlib import Path

if __name__ == '__main__':
    res = subprocess.Popen(['make', '-j4', '-opti="-O2"', 'compiler'],
                           stdout=subprocess.DEVNULL)
    if res.wait():
        cprint("Compile Error!", __bcolor.__ERR)
        exit(-1)
    for (path, folders, files) in walk("third_party/SysYTestCase"):
        for file in files:
            if Path(file).suffix == '.sy':
                sySourceFilePath = path + "/" + file
                res = subprocess.Popen(
                    ['./compiler', sySourceFilePath, '-f', 'frontendParseTest', '-o', '/dev/null'],
                    stdout=subprocess.DEVNULL)
                if res.wait():
                    exit(-1)
    cprint("All Test Passed.", color=__bcolor.__FIN)
