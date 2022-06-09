# About Project

## antlr4 clion support

CLion has its own antlr4 support.

## Google-Test support

See `src/third_party/Google_Test` to get more information.

# Run Compiler

## Make Command

* Compile moecompiler:

    `make -j4 opti="-O2" compiler`

* Compile moecompiler and run:

    `make -j4 opti="-O2" run args="testcase.sysy -S -o testcase.s"`

* Run directly:

    `./compiler testcase.sysy -S -o testcase.s`



## Command Line options

Uasage: `./compiler <sourceFilePath> [-d] (-S|-f <submainName>) -o <OutputFilePath>`

* `[-d]`: To print debug information. (See `mlib/mdb.hpp` for more information)

* `[-f<submainName>]`: Call `<submainName>`. See `mlib/submain.hpp` for more information.

    `-f listall` will list all supported sub main names.



# Clean Build Object Files

`make clean`



# Little Test for Compiler

See `MoeCompiler/notebooks/projNotebooks/projStart.md` for more information.



# Other make Target

## Show Image of parser tree

Usage: `make grun sysy="<testSysYPath>"`

Before use, you should run the commands memtioned in `moe/third_party/README`

* `<testSysYPath>` is the source file you wish to process.



## Run python script

Usage: `make pys pys="<pysName>"`

* `<pysName>`: labelName of python script you wish to execute.

    `listall` will show all supported script names.
