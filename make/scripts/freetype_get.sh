#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

version="2.11.0"

echo "[mechascribe script] downloading freetype v$version source release"
curl -L "https://download.savannah.gnu.org/releases/freetype/freetype-$version.tar.xz" -o \
res/freetype.tar.xz

cd res || exit
tar -xf freetype.tar.xz

rm freetype.tar.xz
mv "freetype-$version" freetype
