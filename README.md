The compiler made for [系统大赛](https://compiler.educg.net).

Structure of this repo:
```txt
.
├── .git
│   └── ...
├── .gitignore
├── moe                     # source code
│   ├── main.cpp            # main file
│   ├── Makefile            # Makefile
│   └── ...
├── notebooks               # some notes
│   ├── docLoad.sh          # shell script for auto downloading
│   ├── docsFromCSC         # notes from CSC
│   ├── gittuto.md          # tutorial for git and github
│   └── README              # README for notebook directory 
├── README.md               # README doc
└── referenceImplementation # implementation of other teams in last two competition
    ├── load.sh             # shell script for auto downloading
    └── README              # README for this directory
```

Compile MoeCompiler (NOT FINISHED):
```shell
cd moe
make -j4 opti="-O2" moecompiler # opti is the argument of level of optimization.
```

