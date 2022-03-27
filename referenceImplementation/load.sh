#!/bin/bash
__CYAN='\033[0;36m'
__GREEN='\033[0;32m'
__NOC='\033[0m'

projDirSpinCat=./SpinCat-BJTU-CSC2020
if ! [ -d $projDirSpinCat ];then
    echo -e "$__CYAN"Loading \"SpinCat\" by BJTU in CSC2020"$__NOC"
    git clone https://gitlab.eduxiji.net/CSC2020-BJTU-SpinCat/compiler.git $projDirSpinCat
fi
echo -e "$__GREEN"Finished."$__NOC"
