#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../..

echo "[mechascribe script] downloading sheenbidi master branch zip"
curl -L "https://github.com/Tehreer/SheenBidi/archive/refs/heads/master.zip" -o \
res/sheenbidi.zip

cd res || exit
unzip sheenbidi.zip

rm sheenbidi.zip
mv SheenBidi-master sheenbidi
