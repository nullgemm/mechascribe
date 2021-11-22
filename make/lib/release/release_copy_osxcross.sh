#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=mechascribe_bin_"$tag"

mkdir -p "$release/lib/mechascribe/macos"
mv bin/mechascribe.a "$release"/lib/mechascribe/macos/mechascribe_macos.a
mv bin/libmechascribe.dylib "$release"/lib/mechascribe/macos/libmechascribe_macos.dylib
