#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

echo "[mechascribe script] downloading libraqm master branch zip"
curl -L "https://github.com/HOST-Oman/libraqm/archive/refs/heads/master.zip" -o \
res/libraqm.zip

cd res || exit
unzip libraqm.zip

rm libraqm.zip
mv libraqm-master libraqm
