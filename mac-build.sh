#!/bin/sh
# libsdl
output="$HOME/Documents/Libraries/SDL"
sdl_src_zip="$HOME/Documents/Libraries/SDL2-*.zip"
if [ -d "$output" ]; then
  echo "Previous sdl2 compilation process output deleted."
  rm -rf $output
fi
symroot="symroot=$output/release"
sdk_device="-sdk iphoneos"
sdk_simulator="-sdk iphonesimulator"
conf_debug="-configuration Debug"
conf_release="-configuration Release"
mkdir -p $output
cd $output
unzip "$sdl_src_zip"
cd *
cd Xcode-iOS/SDL
build_dir=$PWD/build
scheme="libSDL"
proj="-project SDL.xcodeproj"
# xcodebuild OTHER_CFLAGS="-fembed-bitcode" OTHER_LDFLAGS="-lobjc" \
#   ONLY_ACTIVE_ARCH=NO -arch i386 -arch x86_64 $proj $sdk_simulator $conf_debug \
#   -scheme="$scheme" build $symroot
xcodebuild OTHER_CFLAGS="-fembed-bitcode" ONLY_ACTIVE_ARCH=NO \
  -arch i386 -arch x86_64 $proj $sdk_simulator $conf_release -scheme='$scheme' \
  build $symroot
# xcodebuild OTHER_CFLAGS="-fembed-bitcode" OTHER_LDFLAGS="-lobjc" \
#   ONLY_ACTIVE_ARCH=NO -arch arm64 -arch armv7 $proj $sdk_device $conf_debug \
#   -scheme="$scheme" build $symroot
xcodebuild OTHER_CFLAGS="-fembed-bitcode" ONLY_ACTIVE_ARCH=NO -arch arm64 \
  -arch armv7 $proj $sdk_device $conf_release -scheme='$scheme' build $symroot
lipo $build_dir/Release-iphonesimulator/libSDL2.a \
  $build_dir/Release-iphoneos/libSDL2.a -create -output $output/libSDL2.a
# libpng
output="$HOME/Documents/Libraries/PNG"
png_src_zip="$HOME/Documents/Libraries/libpng-*.tar.xz"
if [ -d "$output" ]; then
  echo "Previous png compilation process output deleted."
  rm -rf $output
fi
mkdir -p $output
cd $output
tar xf $png_src_zip
cd *
png_src=$PWD
cp scripts/makefile.darwin Makefile
make -j `sysctl -n hw.ncpu`
