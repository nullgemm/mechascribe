#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

build=$1
library=$2

tag=$(git tag --sort v:refname | tail -n 1)

# library makefile data
cc="x86_64-w64-mingw32-gcc"
makefile="makefile_example_windows"
name="mechascribe_example_windows"
mechascribe="mechascribe_windows"

src+=("example/example.c")
src+=("example/glass.c")

flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Wno-implicit-fallthrough")
flags+=("-Wno-cast-function-type")
flags+=("-Wno-incompatible-pointer-types")
flags+=("-Imechascribe_bin_$tag/include")
flags+=("-Ires/libspng/spng")
flags+=("-Iexample")

defines+=("-DEXAMPLE_PLATFORM_MINGW")
defines+=("-DUNICODE")
defines+=("-D_UNICODE")
defines+=("-DWINVER=0x0A00")
defines+=("-D_WIN32_WINNT=0x0A00")
defines+=("-DCINTERFACE")
defines+=("-DCOBJMACROS")

# generated linker arguments
ldflags+=("-fno-stack-protector")
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
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
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
obj+=("mechascribe_bin_$tag/lib/mechascribe/windows/""$mechascribe""_mingw.a")
obj+=("res/hydraquill/lib/hydraquill/windows/hydraquill_windows_mingw.a")
obj+=("res/libraqm/build/src/libraqm.a")
obj+=("res/harfbuzz/build/src/libharfbuzz.a")
obj+=("res/freetype/build/libfreetype.a")
obj+=("res/sheenbidi/build/libsheenbidi.a")
obj+=("res/libspng/build/libspng.a")
defines+=("-DSPNG_STATIC")
ldlibs+=("-lm")
ldlibs+=("-lz")
cmd="./$name.exe"
	;;

	shared)
obj+=("mechascribe_bin_$tag/lib/mechascribe/windows/""$mechascribe""_mingw.dll.a")
obj+=("res/hydraquill/lib/hydraquill/windows/hydraquill_windows_mingw.dll.a")
obj+=("res/libraqm/build/src/libraqm.dll.a")
obj+=("res/harfbuzz/build/src/libharfbuzz.dll.a")
obj+=("res/freetype/build/libfreetype.dll.a")
obj+=("res/sheenbidi/build/libsheenbidi.dll.a")
obj+=("res/libspng/build/libspng.a")
defines+=("-DSPNG_STATIC")
ldlibs+=("-lm")
ldlibs+=("-lz")
cmd="../make/scripts/dll_copy.sh ""$mechascribe""_mingw.dll && ./$name.exe"
	;;

	*)
echo "invalid library type"
exit 1
	;;
esac

# default target
default+=("\$(OBJ_EXTRA)")
default+=("bin/\$(NAME)")

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

# makefile default target
echo "" >> $makefile
echo "default:" "${default[@]}" >> $makefile

# makefile linux targets
echo "" >> $makefile
cat make/example/templates/targets_windows_mingw.make >> $makefile

# makefile object targets
echo "" >> $makefile
for file in "${src[@]}"; do
	$cc "${defines[@]}" -MM -MG "$file" >> $makefile
done

# makefile extra targets
echo "" >> $makefile
cat make/example/templates/targets_extra.make >> $makefile
