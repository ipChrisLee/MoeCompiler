# About Project

## antlr4 clion support

CLion has its own antlr4 support.

## Google-Test support

See `src/third_party/Google_Test` to get more information.

# Run Compiler

## Make Command

* Compile moe compiler:

  `mkdir build;cd build;cmake ..;make -j4;`

* Run:

    `./compiler testcase.sy -S -o testcase.ms`

# About File Type
* `.ms`: moe assembly file.
* `.mll`: moe llvm-ir file.
* `.mout`: moe output file.
