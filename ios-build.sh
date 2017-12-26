#!/bin/sh
cpu_threads=`sysctl -n hw.ncpu`
arg_sdl="sdl"
arg_png="png"
# libsdl
if [[ $arg_sdl = $1 || $arg_sdl = $2 ]]; then
  output="$HOME/Documents/Libraries/SDL"
  sdl_src_zip="$HOME/Documents/Libraries/SDL2-*.zip"
  if [ -d "$output" ]; then
    echo "Previous sdl2 compilation process output deleted."
    rm -rf $output
  fi
  symroot="symroot=$output/release"
  sdk_device="-sdk iphoneos"
  sdk_mac="-sdk macosx"
  sdk_simulator="-sdk iphonesimulator"
  conf_debug="-configuration Debug"
  conf_release="-configuration Release"
  mkdir -p $output
  cd $output
  unzip "$sdl_src_zip"
  cd *
  cd Xcode-iOS/SDL
  ios_build_dir=$PWD/build
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
  cd ../../Xcode/SDL
  mac_build_dir=$PWD/build
  xcodebuild OTHER_CFLAGS="-fembed-bitcode" ONLY_ACTIVE_ARCH=NO -arch x86_64 \
    -arch i386 $proj $sdk_mac $conf_release -scheme='$scheme' build $symroot
  lipo $ios_build_dir/Release-iphonesimulator/libSDL2.a \
       $ios_build_dir/Release-iphoneos/libSDL2.a \
       -create -output $output/libSDL2.a
fi;
# libpng
if [[ $arg_png = $1 || $arg_png = $2 ]]; then
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
  # build simulator
  ./configure --prefix=`pwd`/install_i386 CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphonesimulator clang -arch i386 -fembed-bitcode-marker -fembed-bitcode" --host=x86
  make clean && make -j$cpu_threads && make install

  ./configure --prefix=`pwd`/install_x86_64 CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphonesimulator clang -arch x86_64 -fembed-bitcode-marker -fembed-bitcode" --host=x86_64
  make clean && make -j$cpu_threads && make install

  # build armv7
  ./configure --prefix=`pwd`/install_armv7 --enable-arm-neon=api CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7 -fembed-bitcode-marker -fembed-bitcode" --host=arm
  make clean && make -j$cpu_threads && make install

  # build armv7s
  ./configure --prefix=`pwd`/install_armv7s --enable-arm-neon=api CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7s -fembed-bitcode-marker -fembed-bitcode" --host=arm
  make clean && make -j$cpu_threads && make install

  # build armv7s
  ./configure --prefix=`pwd`/install_arm64 --enable-arm-neon=api CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch arm64 -fembed-bitcode-marker -fembed-bitcode" --host=arm
  make clean && make -j$cpu_threads && make install

  # build universal
  mkdir -p output-ios
  xcrun lipo -create $(find install_*/lib -name "libpng.a") -o output-ios/libpng.a
  cp -r install_armv7/include output-ios/.

  # build macosx
  ./configure --prefix=`pwd`/install_macosx CFLAGS="-Ofast -mmacosx-version-min=10.7" LDFLAGS="-flto" CC="xcrun -sdk macosx clang -arch x86_64"
  make clean && make -j$cpu_threads && make install
  install_name_tool -id "@executable_path/../Frameworks/libpng.dylib" install_macosx/lib/libpng.dylib

  mkdir -p output-macosx
  cp -r install_macosx/include output-macosx/.
  LIB=libpng.dylib; while readlink install_macosx/lib/$LIB > /dev/null; do LIB=$(readlink install_macosx/lib/$LIB); done;
  cp install_macosx/lib/$LIB output-macosx/libpng.dylib
fi;
