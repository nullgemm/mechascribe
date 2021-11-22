#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

cp mechascribe_bin_*/lib/mechascribe/windows/"$1" bin/mechascribe.dll
cp res/hydraquill/lib/hydraquill/windows/hydraquill_windows_mingw.dll bin/hydraquill.dll
cp res/libraqm/build/src/libraqm-0.dll bin/
cp res/harfbuzz/build/src/libharfbuzz-0.dll bin/
cp res/freetype/build/libfreetype-6.dll bin/
cp res/sheenbidi/build/libsheenbidi-2.dll bin/
