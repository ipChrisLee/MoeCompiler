#!/bin/bash
source ./scripts/shScripts/settings.sh

"${moe}" "${syBaseFilePath}".sy -S -o "${syBaseFilePath}".mll --emit-llvm

llc "${syBaseFilePath}".mll -o "${syBaseFilePath}".ms
