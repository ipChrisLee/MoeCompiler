#!/bin/bash
__CYAN='\033[0;36m'
__GREEN='\033[0;32m'
__NOC='\033[0m'

projDirSpinCat=./SpinCat-BJTU-CSC2020
if ! [ -d $projDirSpinCat ];then
    echo -e "$__CYAN"Loading \"SpinCat\" by BJTU in CSC2020"$__NOC"
    git clone https://gitlab.eduxiji.net/CSC2020-BJTU-SpinCat/compiler.git $projDirSpinCat
fi

projDirKobayashi=./Kobayashi-THU-CSC2021
if ! [ -d $projDirKobayashi ];then
    echo -e "$__CYAN"Loading \"Kobayashi-Compiler\" by THU \(rk1\) in CSC2021"$__NOC"
    git clone https://github.com/kobayashi-compiler/kobayashi-compiler.git $projDirKobayashi
fi

projDirMahoShojo=./MahoShojo-USTC-CSC2021
if ! [ -d $projDirMahoShojo ];then
    echo -e "$__CYAN"Loading \"MahoShojo\" by USTC \(rk8\) in CSC2021"$__NOC"
    git clone https://github.com/wildoranges/MahoShojo.git $projDirMahoShojo
fi

projDirFlamming=./Flamming-USTC-CSC2020
if ! [ -d $projDirFlamming ];then
    echo -e "$__CYAN"Loading \"FlammingMyCompiler\" by USTC \(rk1\) in CSC2020"$__NOC"
    git clone https://github.com/mlzeng/CSC2020-USTC-FlammingMyCompiler.git $projDirFlamming
fi

echo -e "$__GREEN"Finished."$__NOC"

