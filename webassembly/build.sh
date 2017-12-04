#!/bin/bash
process="$0"
build_name="build.sh"
gearoenix_dir="${process%$build_name}.."
src_dir="$gearoenix_dir/gearoenix"
output_dir="$gearoenix_dir/output"
asset_dir="$gearoenix_dir/assets"
echo $output_dir
mkdir -p $output_dir
echo "cd $output_dir"
cd $output_dir
cpps=$(find ../ -iname *.cpp | xargs echo)
source $EMSCRIPTEN_SDK/emsdk_env.sh
em++ -s WASM=1 -std=c++14 -s BINARYEN=1 -s USE_WEBGL2=1 -s USE_SDL=2 --emrun \
    --preload-file data.gx3d --use-preload-plugins -o index.html -O3 $cpps
