<<<<<<< HEAD
The compiler made for [CSC2022](https://compiler.educg.net).

Source language: [sysy (subset of C)](https://gitlab.eduxiji.net/nscscc/compiler2022/-/blob/master/SysY2022语言定义-V1.pdf)
Target: ARMv7 

Compile MoeCompiler (NOT FINISHED):
```shell
cd moe
mkdir build
cd build
cmake ..
make -j4
```

Read "moe/README" for more information.

=======
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
>>>>>>> d09887b (FRONTEND FINISHED!)
