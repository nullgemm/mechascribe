#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="0.7.0"

echo "[mechascribe script] downloading libspng v$version source release"
curl -L "https://github.com/randy408/libspng/archive/refs/tags/v$version.zip" -o \
res/libspng.zip

cd res || exit
unzip libspng.zip

rm libspng.zip
mv "libspng-$version" libspng
