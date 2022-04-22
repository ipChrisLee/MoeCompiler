from os import path
import sys


myLibs = ['mlib/', 'clib/', '/', 'third_party/antlr4/']


def findProjRoot(filename):
    ph = path.dirname(filename)
    while ph != '/':
        if path.exists(ph+'/.ycm_extra_conf.py'):
            return ph
        ph = path.dirname(ph)
    raise FileNotFoundError('Don\'t found .ycm_extra_conf.py file.')


def Settings(**kwargs):
    _dbg = 0
    if _dbg:
        return {
            'flags': [
                'x', 'c++',
                '-Wall', '-Wextra', '-Werror',
                '-std=c++17', '-D CODING'
            ]
        }

    projRootDir = findProjRoot(kwargs['filename'])
    print(projRootDir)
    ilibs = ['-I'+str(projRootDir)+'/'+_ for _ in myLibs]
    flags = ['x', 'c++',
             '-Wall', '-Wextra', '-Werror',
             '-std=c++17', '-DCODING']+ilibs
    print(str(flags))
    return {'flags': flags}
