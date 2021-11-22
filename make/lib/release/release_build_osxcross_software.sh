#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

./make/lib/gen/gen_macos.sh release osxcross osxcross

make -f makefile_lib_macos clean

if [[ -v AR ]]; then
	make -f makefile_lib_macos -e AR="$AR"
else
	make -f makefile_lib_macos
fi
