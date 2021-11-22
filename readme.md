# Mechascribe
Mechascribe is a text rendering abstraction trying to support as much scripts as
possible by combining the best publicly available libraries. Among its features:
 - support for complex text layout (library used: Raqm)
 - feature-complete rendering (library used: Freetype)
 - very fast software rendering (library used: Freetype)
 - modern 2-step CPU+GPU rendering (library used: Freetype)
 - simplified distribution of the Noto fonts for extensive unicode coverage
 - completely independent font fallback management
 - simple API

Advantages of this approach:
 - Cross-platform - We take care of the portability issues
 - All-inclusive - Every step of the rendering process is covered

Disadvantages of this approach:
 - No system integration - You must provide the fonts and scaling information
 - Reduced creative control - Effects can only happen after rendering the glyphs

## Dependencies
 - Raqm (layout, depends on Harfbuzz and SheenBidi)
   Using Harfbuzz and SheenBidi is very far from being straightforward.
   Raqm wraps around them and saves us from writing a lot of code,
   generating the final layout and outputting glyph positions.
 - Freetype (rendering)
   Text rendering is hard to accelerate, and very hard to make feature-complete.
   Sadly, the best font rendering libraries available today are simultaneously
   proprietary, patent-encumbered, and outrageously expansive.
   Freetype is a qualitative, open source and feature-complete library.
   It is also well-established, cross-platform, fast, and can even render
   SDF representations of the glyphs for use on GPU with pixel shaders.
 - Hydraquill (Noto distribution)
   The first step to supporting as much scripts as possible consists in
   gathering a set of fonts covering all the required glyphs. The Noto
   fonts were created by Google to solve this exact issue.
   The complete collection weights more than 1.5GiB, but keeping only the
   regular-weight variable-width sans-serif variants makes it only 74MiB.
   Zstd compression can help us reduce its size even further: using its
   most aggressive parameters, our set can be made to fit a 16MiB file.
   While the compression takes quite some time even on a powerful computer,
   the resulting archive still unpacks almost instantaneously.
   I wrote Hydraquill to take advantage of Zstd's interesting characteristics
   and help distribute a super-compressed Noto archive. It assists unpacking,
   checking and processing the font files by providing all the utilitary
   code required to perform these tasks in a portable way.
 - Zstd
   Zstd's decompressor is used to unpack the main font archive.
 - Cifra
   Cifra's public-domain sha256 implementation is used for all checksums.

## Building
### Prerequisites & Notes
#### Linux
Install the following packages to be able to compile the dependencies:
 - `meson`

While using LLVM's `lld` could result in better build times for the example,
I was only able to link some meson-compiled dependencies with GNU's `ld`
(even when compiling everything using LLVM's toolchain).

#### Windows
Install the following packages to be able to compile the dependencies:
 - `mingw-w64-x86_64-meson` (regular `meson` does not generate valid binaries)

#### macOS
Install the following packages to be able to compile the dependencies:
 - `meson`
 - `pkg-config`

### Building manually
First, download and build all the required dependencies locally:
```
./make/scripts/prepare.sh
```

Now, run the makefile generators in `./make/lib/gen`.
They are interactive by default, but can be automated easily
(simply pass the answers as arguments to the script, in the right order).
Interactive example for Linux:
```
./make/lib/gen/gen_linux.sh
```
Automated example for Linux:
```
./make/lib/gen/gen_linux.sh development
```

Then, run the generated makefile and grab the library in the `bin` folder:
```
make -f makefile_lib_linux
```

### Building releases automatically
If you simply want to build a release version of the library you can also run
the scripts in `./make/lib/release`, which automate the entire building process.
In this case the makefiles will be generated automatically but you still have
to run `./make/scripts/prepare.sh`, to get all the dependencies before building.

Complete example for the Linux part of a release:
```
./make/scripts/prepare.sh
./make/lib/release/release_build_linux.sh
./make/lib/release/release_copy_linux.sh
./make/lib/release/release_headers.sh
```
