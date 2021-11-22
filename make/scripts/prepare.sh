#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit

./freetype_get.sh
./freetype_gen.sh

./harfbuzz_get.sh
./harfbuzz_gen.sh

./libraqm_get.sh
./libraqm_gen.sh

./libspng_get.sh
./libspng_gen.sh

./sheenbidi_get.sh
./sheenbidi_gen.sh

./zstd_get.sh
./zstd_gen.sh

./hydraquill_get.sh
./noto_get.sh
