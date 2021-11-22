#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

# build libraqm
cd res/sheenbidi || exit

echo "[mechascribe script] building sheenbidi"
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
	-Derrorlogs=false \
	-Dstdsplit=false

ninja -C build
