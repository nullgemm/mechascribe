#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
library=$2

tag=$(git tag --sort v:refname | tail -n 1)

# library makefile data
cc="gcc"
makefile="makefile_example_linux"
name="mechascribe_example_linux"
mechascribe="mechascribe_linux"

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

defines+=("-D_DEFAULT_SOURCE")
defines+=("-DEXAMPLE_PLATFORM_LINUX")

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

# link type
if [ -z "$library" ]; then
	read -rp "select library type (static | shared): " library
fi

case $library in
	static)
obj+=("mechascribe_bin_$tag/lib/mechascribe/linux/$mechascribe.a")
obj+=("res/hydraquill/lib/hydraquill/linux/hydraquill_linux.a")
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
ldflags+=("-Lmechascribe_bin_$tag/lib/mechascribe/linux")
ldflags+=("-Lres/hydraquill/lib/hydraquill/linux")
ldflags+=("-Lres/libspng/build")
ldflags+=("-Lres/libraqm/build/src")
ldflags+=("-Lres/harfbuzz/build/src")
ldflags+=("-Lres/freetype/build")
ldflags+=("-Lres/sheenbidi/build")
ldlibs+=("-l:$mechascribe.so")
ldlibs+=("-l:hydraquill_linux.so")
ldlibs+=("-l:libspng.so")
ldlibs+=("-l:libraqm.so")
ldlibs+=("-l:libharfbuzz.so")
ldlibs+=("-l:libfreetype.so")
ldlibs+=("-l:libsheenbidi.so")
cmd+="LD_LIBRARY_PATH="
cmd+="../mechascribe_bin_$tag/lib/mechascribe/linux:"
cmd+="../res/hydraquill/lib/hydraquill/linux:"
cmd+="../res/libspng/build:"
cmd+="../res/libraqm/build/src:"
cmd+="../res/harfbuzz/build/src:"
cmd+="../res/freetype/build:"
cmd+="../res/sheenbidi/build"
cmd+=" ./$name"
	;;

	*)
echo "invalid library type"
exit 1
	;;
esac

# default target
default+=("\$(OBJ_EXTRA)")
default+=("bin/\$(NAME)")

# valgrind flags
valgrind+=("--show-error-list=yes")
valgrind+=("--show-leak-kinds=all")
valgrind+=("--track-origins=yes")
valgrind+=("--leak-check=full")

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CMD = $cmd"; \
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
cat make/example/templates/targets_linux.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/example/templates/targets_extra.make >> $makefile
