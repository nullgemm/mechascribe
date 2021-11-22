#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
toolchain=$2
current_toolchain=$3

# library makefile data
makefile="makefile_lib_macos"
name="mechascribe"

src+=("src/noto.c")
src+=("src/mechascribe.c")
src+=("res/cifra/src/sha256.c")
src+=("res/cifra/src/blockwise.c")
src+=("res/zstd/build/single_file_libs/zstddeclib.c")

ldflags+=("-Lres/hydraquill/lib/hydraquill/macos")
ldflags+=("-Lres/libspng/build")
ldflags+=("-Lres/libraqm/build/src")
ldflags+=("-Lres/harfbuzz/build/src")
ldflags+=("-Lres/freetype/build")
ldflags+=("-Lres/sheenbidi/build")
ldlibs+=("-lhydraquill_macos")
ldlibs+=("-lspng")
ldlibs+=("-lraqm")
ldlibs+=("-lharfbuzz")
ldlibs+=("-lfreetype")
ldlibs+=("-lsheenbidi")

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
defines+=("-DMECHASCRIBE_PLATFORM_MACOS")

# build type
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
flags+=("-O2")
	;;

	sanitized)
flags+=("-g")
flags+=("-fno-omit-frame-pointer")
flags+=("-fPIE")
flags+=("-O2")
flags+=("-fsanitize=undefined")
flags+=("-fsanitize=function")
ldflags+=("-fsanitize=undefined")
ldflags+=("-fsanitize=function")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("bin/$name.a")
default+=("bin/lib$name.dylib")

# toolchain type
if [ -z "$toolchain" ]; then
	read -rp "select target toolchain type (osxcross | native): " toolchain
fi

case $toolchain in
	osxcross)
cc="o64-clang"
ar="x86_64-apple-darwin20.2-ar"
	;;

	native)
makefile+="_native"
cc="clang"
ar="ar"
	;;

	*)
echo "invalid target toolchain type"
exit 1
	;;
esac

if [ -z "$current_toolchain" ]; then
	read -rp "select current toolchain type (osxcross | native): " current_toolchain
fi

case $current_toolchain in
	osxcross)
cch="o64-clang"
	;;

	native)
cch="clang"
	;;

	*)
echo "invalid current toolchain type"
exit 1
	;;
esac

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CC = $cc"; \
echo "AR = $ar"; \
} > $makefile

# makefile linking info
echo "" >> $makefile
for flag in "${ldflags[@]}"; do
	echo "LDFLAGS+= $flag" >> $makefile
done

echo "" >> $makefile
for flag in "${ldlibs[@]}"; do
	echo "LDLIBS+= $flag" >> $makefile
done

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
cat make/lib/templates/targets_macos.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cch "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
