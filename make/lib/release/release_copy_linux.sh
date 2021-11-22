#!/bin/bash

# get into the right folder
cd "$(dirname "$0")" || exit
cd ../../..

tag=$(git tag --sort v:refname | tail -n 1)
release=mechascribe_bin_"$tag"

mkdir -p "$release/lib/mechascribe/linux"
mv bin/mechascribe.a "$release"/lib/mechascribe/linux/mechascribe_linux.a
mv bin/mechascribe.so "$release"/lib/mechascribe/linux/mechascribe_linux.so
