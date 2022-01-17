#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="0.8.0"

echo "[mechascribe script] downloading libraqm v$version source release"
curl -L "https://github.com/HOST-Oman/libraqm/releases/download/v$version/raqm-$version.tar.xz" -o \
res/libraqm.tar.xz

cd res || exit
tar -xf libraqm.tar.xz

rm libraqm.tar.xz
mv "raqm-$version" libraqm
