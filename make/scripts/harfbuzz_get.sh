#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="2.9.0"

echo "[mechascribe script] downloading harfbuzz v$version source release"
curl -L "https://github.com/harfbuzz/harfbuzz/releases/download/$version/harfbuzz-$version.tar.xz" -o \
res/harfbuzz.tar.xz

cd res || exit
tar -xf harfbuzz.tar.xz

rm harfbuzz.tar.xz
mv "harfbuzz-$version" harfbuzz
