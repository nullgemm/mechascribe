#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

./make/lib/gen/gen_linux.sh release
make -f makefile_lib_linux clean
make -f makefile_lib_linux
