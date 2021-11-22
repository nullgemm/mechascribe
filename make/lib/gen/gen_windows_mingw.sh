#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1

# library makefile data
makefile="makefile_lib_windows"
name="mechascribe"
cc="x86_64-w64-mingw32-gcc"

src+=("src/noto.c")
src+=("src/mechascribe.c")
src+=("res/cifra/src/sha256.c")
src+=("res/cifra/src/blockwise.c")
src+=("res/zstd/build/single_file_libs/zstddeclib.c")

objdll+=("res/hydraquill/lib/hydraquill/windows/hydraquill_windows_mingw.dll.a")
objdll+=("res/libspng/build/libspng.dll.a")
objdll+=("res/libraqm/build/src/libraqm.dll.a")
objdll+=("res/harfbuzz/build/src/libharfbuzz.dll.a")
objdll+=("res/freetype/build/libfreetype.dll.a")
objdll+=("res/sheenbidi/build/libsheenbidi.dll.a")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-implicit-fallthrough")
flags+=("-Wno-cast-function-type")
flags+=("-Wno-incompatible-pointer-types")
flags+=("-Isrc")
flags+=("-Ires/cifra/src")
flags+=("-Ires/freetype/include")
flags+=("-Ires/hydraquill/include")
flags+=("-Ires/libraqm/build/src")
flags+=("-Ires/libraqm/src")
flags+=("-Ires/zstd/lib")
flags+=("-fPIC")

# library platform
defines+=("-DMECHASCRIBE_PLATFORM_MINGW")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")
defines+=("-DCINTERFACE")
defines+=("-DCOBJMACROS")

# generated linker arguments
ldlibs+=("-mwindows")

# build type
if [ -z "$build" ]; then
	read -rp "select build type (development | release): " build
fi

case $build in
	development)
flags+=("-g")
	;;

	release)
flags+=("-O2")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# add the libraries as default targets
default+=("bin/$name.dll")
default+=("bin/$name.a")

# makefile start
{ \
echo ".POSIX:"; \
echo "NAME = $name"; \
echo "CC = $cc"; \
} > $makefile

# makefile linking info
#echo "" >> $makefile
#for flag in "${ldflags[@]}"; do
	#echo "LDFLAGS+= $flag" >> $makefile
#done

#echo "" >> $makefile
#for flag in "${ldlibs[@]}"; do
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

echo "" >> $makefile
for file in "${objdll[@]}"; do
	echo "OBJDLL+= $file" >> $makefile
done

# makefile default target
echo "" >> $makefile
echo "default:" "${default[@]}" >> $makefile

# makefile library targets
echo "" >> $makefile
cat make/lib/templates/targets_windows_mingw.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/lib/templates/targets_extra.make >> $makefile
