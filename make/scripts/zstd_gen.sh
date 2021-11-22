#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

echo "[mechascribe script] building zstd decoder"
cd res/zstd/build/single_file_libs || exit
./create_single_file_decoder.sh
