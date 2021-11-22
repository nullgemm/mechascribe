#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1

# library makefile data
makefile="makefile_lib_linux"
name="mechascribe"
cc="gcc"

src+=("src/noto.c")
src+=("src/mechascribe.c")
src+=("res/cifra/src/sha256.c")
src+=("res/cifra/src/blockwise.c")
src+=("res/zstd/build/single_file_libs/zstddeclib.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-Ires/cifra/src")
flags+=("-Ires/freetype/include")
flags+=("-Ires/hydraquill/include")
flags+=("-Ires/libraqm/build/src")
flags+=("-Ires/libraqm/src")
flags+=("-Ires/zstd/lib")
flags+=("-fPIC")

# library platform
defines+=("-DMECHASCRIBE_PLATFORM_LINUX")

if [ -z "$build" ]; then
	read -rp "select build type (development | release | sanitized): " build
fi

case $build in
	development)
flags+=("-g")
	;;

	release)
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fstack-protector-strong")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
ldflags+=("-z relro")
ldflags+=("-z now")
	;;

	sanitized)
cc="clang"
flags+=("-g")
flags+=("-fno-omit-frame-pointer")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
flags+=("-fsanitize=undefined")
flags+=("-mllvm")
flags+=("-msan-keep-going=1")
flags+=("-fsanitize=memory")
flags+=("-fsanitize-memory-track-origins=2")
flags+=("-fsanitize=function")
ldflags+=("-fsanitize=undefined")
ldflags+=("-fsanitize=memory")
ldflags+=("-fsanitize-memory-track-origins=2")
ldflags+=("-fsanitize=function")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("bin/$name.a")
default+=("bin/$name.so")

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CC = $cc"; \
} > $makefile

# makefile linking info
#echo "" >> $makefile
#for flag in $(pkg-config "${link[@]}" --cflags) "${ldflags[@]}"; do
	#echo "LDFLAGS+= $flag" >> $makefile
#done

#echo "" >> $makefile
#for flag in $(pkg-config "${link[@]}" --libs) "${ldlibs[@]}"; do
	#echo "LDLIBS+= $flag" >> $makefile
#done

# makefile compiler flags
echo "" >> $makefile
for flag in "${flags[@]}"; do
	echo "CFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for define in "${defines[@]}"; do
	echo "CFLAGS+= $define" >> $makefile
done

# makefile object list
echo "" >> $makefile
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .c)
	echo "OBJ+= $folder/$filename.o" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default:" "${default[@]}" >> $makefile

# makefile library targets
echo "" >> $makefile
cat make/lib/templates/targets_linux.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
