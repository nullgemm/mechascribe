#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
toolchain=$2
current_toolchain=$3
library=$4

tag=$(git tag --sort v:refname | tail -n 1)

# library makefile data
makefile="makefile_example_macos"
name="mechascribe_example_macos"
mechascribe="mechascribe_macos"

src+=("example/example.c")
src+=("example/glass.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Imechascribe_bin_$tag/include")
flags+=("-Ires/libspng/spng")
flags+=("-Iexample")

defines+=("-DEXAMPLE_PLATFORM_MACOS")

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

# toolchain type
if [ -z "$toolchain" ]; then
	read -rp "select target toolchain type (osxcross | native): " toolchain
fi

case $toolchain in
	osxcross)
cc="o64-clang"
objcopy="objcopy"
	;;

	native)
makefile+="_native"
name+="_native"
mechascribe+="_native"
cc="clang"
objcopy="/usr/local/Cellar/binutils/*/bin/objcopy"
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

# link type
if [ -z "$library" ]; then
	read -rp "select library type (static | shared): " library
fi

case $library in
	static)
obj+=("mechascribe_bin_$tag/lib/mechascribe/macos/$mechascribe.a")
obj+=("res/hydraquill/lib/hydraquill/macos/hydraquill_macos.a")
obj+=("res/libspng/build/libspng.a")
obj+=("res/libraqm/build/src/libraqm.a")
obj+=("res/harfbuzz/build/src/libharfbuzz.a")
obj+=("res/freetype/build/libfreetype.a")
obj+=("res/sheenbidi/build/libsheenbidi.a")
defines+=("-DSPNG_STATIC")
ldlibs+=("-lm")
ldlibs+=("-lz")
cmd="./$name"
	;;

	shared)
ldflags+=("-Lmechascribe_bin_$tag/lib/mechascribe/macos")
ldflags+=("-Lres/hydraquill/lib/hydraquill/macos")
ldflags+=("-Lres/libspng/build")
ldflags+=("-Lres/libraqm/build/src")
ldflags+=("-Lres/harfbuzz/build/src")
ldflags+=("-Lres/freetype/build")
ldflags+=("-Lres/sheenbidi/build")
ldlibs+=("-l$mechascribe")
ldlibs+=("-lhydraquill_macos")
ldlibs+=("-lspng")
ldlibs+=("-lraqm")
ldlibs+=("-lharfbuzz")
ldlibs+=("-lfreetype")
ldlibs+=("-lsheenbidi")
cmd+="../make/scripts/dylib_copy.sh $mechascribe"
cmd+=" && DYLD_INSERT_LIBRARIES="
cmd+="../res/libspng/build/libspng.dylib:"
cmd+="../res/libraqm/build/src/libraqm.dylib:"
cmd+="../res/harfbuzz/build/src/libharfbuzz.dylib:"
cmd+="../res/freetype/build/libfreetype.dylib:"
cmd+="../res/sheenbidi/build/libsheenbidi.dylib"
cmd+=" ./$name"
	;;

	*)
echo "invalid library type"
exit 1
	;;
esac

# default target
default+=("\$(OBJ_EXTRA)")
default+=("bin/\$(NAME).app")

# valgrind flags
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all")
valgrind+=("--track-origins=yes")
valgrind+=("--leak-check=full")
valgrind+=("--suppressions=../res/valgrind.supp")

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CMD = $cmd"; \
echo "OBJCOPY = $objcopy"; \
echo "CC = $cc"; \
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

echo "" >> $makefile
for prebuilt in "${obj[@]}"; do
	echo "OBJ_EXTRA+= $prebuilt" >> $makefile
done

# generate valgrind flags
echo "" >> $makefile
for flag in "${valgrind[@]}"; do
	echo "VALGRIND+= $flag" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default:" "${default[@]}" >> $makefile

# makefile linux targets
echo "" >> $makefile
cat make/example/templates/targets_macos.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cch "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/example/templates/targets_extra.make >> $makefile
