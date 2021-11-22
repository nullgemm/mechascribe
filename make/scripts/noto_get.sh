#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="v0.3.3"

echo "[mechascribe script] downloading hydraquill $version noto pack"
curl -L "https://github.com/nullgemm/hydraquill/releases/download/$version/noto_pack.tar" -o \
res/noto_pack.tar

cd res || exit
tar -xf noto_pack.tar
rm noto_pack.tar
