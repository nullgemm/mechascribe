#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="v0.3.3"

echo "[mechascribe script] downloading hydraquill $version binary release"
curl -L "https://github.com/nullgemm/hydraquill/releases/download/$version/hydraquill_bin_$version.zip" -o \
res/hydraquill.zip

cd res || exit
unzip hydraquill.zip

rm hydraquill.zip
mv "hydraquill_bin_$version" hydraquill
