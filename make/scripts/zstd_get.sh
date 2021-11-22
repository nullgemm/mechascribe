#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="1.5.0"

echo "[mechascribe script] downloading zstd v$version source release"
curl -L "https://github.com/facebook/zstd/releases/download/v$version/zstd-$version.tar.zst" -o \
res/zstd.tar.zst

cd res || exit
unzstd zstd.tar.zst
tar -xf zstd.tar

rm zstd.tar.zst zstd.tar
mv "zstd-$version" zstd
