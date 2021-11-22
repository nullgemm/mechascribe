#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

cp mechascribe_bin_*/lib/mechascribe/macos/lib"$1".dylib bin/libmechascribe.dylib
cp res/hydraquill/lib/hydraquill/macos/libhydraquill_macos.dylib bin/libhydraquill.dylib
