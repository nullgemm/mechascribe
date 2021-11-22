#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

# build libspng
cd res/libspng || exit

echo "[mechascribe script] building libspng"
meson build \
	-Dauto_features=disabled \
	-Dbackend=ninja \
	-Dbuildtype=release \
	-Ddebug=false \
	-Ddefault_library=both \
	-Doptimization=3 \
	-Dwrap_mode=forcefallback \
	-Db_lto=true \
	-Db_lto_threads=16 \
	-Db_ndebug=true \
	-Dbuild_examples=false

ninja -C build
