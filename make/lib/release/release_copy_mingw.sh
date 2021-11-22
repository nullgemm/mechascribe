#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=mechascribe_bin_"$tag"

mkdir -p "$release/lib/mechascribe/windows"
mv bin/mechascribe.a "$release"/lib/mechascribe/windows/mechascribe_windows_mingw.a
mv bin/mechascribe.dll.a "$release"/lib/mechascribe/windows/mechascribe_windows_mingw.dll.a
mv bin/mechascribe.dll "$release"/lib/mechascribe/windows/mechascribe_windows_mingw.dll
