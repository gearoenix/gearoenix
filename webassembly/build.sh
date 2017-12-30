#!/bin/bash
clear
clear
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
find ./ -type f -not -name 'data.gx3d' -delete
# cpps=$(find ../ -iname *.hpp -o -iname *.cpp | xargs echo)
source $EMSCRIPTEN_SDK/emsdk_env.sh
# find ../gearoenix/ -iname *.hpp | while read line; do
#   hpp=${line:13}
#   hpp=${hpp//"/"/"-"}
#   echo "compiling header file $line to $hpp.o"
#   em++ -std=c++14 -c -s USE_SDL=2 -g $line -o $hpp.o
# done
find ../gearoenix/ -iname *.cpp | while read line; do
  cppfile=${line:13}
  cppfile=${cppfile//"/"/"-"}
  echo "compiling header file $line to $cppfile.bc"
  em++ -std=c++14 -s WASM=1 -s BINARYEN=1 -s USE_SDL=2 -s NO_EXIT_RUNTIME=1 \
       -s ALLOW_MEMORY_GROWTH=1 \
       -Oz -c $line -o $cppfile.bc
done
em++ -std=c++14 -s WASM=1 -s BINARYEN=1 -s USE_SDL=2 -s NO_EXIT_RUNTIME=1 \
     -s ALLOW_MEMORY_GROWTH=1 --embed-file data.gx3d \
     --shell-file ../webassembly/index.html \
     -Oz *.bc -o index.html
rm *.bc

# -s WASM=1 -s BINARYEN=1

# if [ $1 == "d" ]; then
#   emcc -std=c++14 -s USE_SDL=2 --embed-file data.gx3d -o index.html -g $cpps
# else
#   emcc -s WASM=1 -std=c++14 -s BINARYEN=1 -s USE_WEBGL2=1 -s USE_SDL=2 --emrun \
#     --embed-file data.gx3d -o index.html -O3 $cpps
# fi
