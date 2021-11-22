#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

# build harfbuzz
cd res/harfbuzz || exit

echo "[mechascribe script] building harfbuzz"
meson build \
	-Dauto_features=disabled \
	-Dbackend=ninja \
	-Dbuildtype=release \
	-Ddebug=false \
	-Ddefault_library=both \
	-Doptimization=3 \
	-Dwrap_mode=forcefallback \
	-Db_lto=true \
	-Db_lto_thread=16 \
	-Db_ndebug=true \
	-Dfreetype=enabled \
	-Dtests=disabled

ninja -C build
