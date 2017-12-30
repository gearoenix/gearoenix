#!/bin/bash
process="$0"
build_name="linux-build.sh"
gearoenix_dir="${process%$build_name}"
src_dir="$gearoenix_dir/gearoenix"
output_dir="$gearoenix_dir/output"
echo $output_dir
mkdir -p $output_dir
echo "cd $output_dir"
cd $output_dir
find ./ -type f -not -name 'data.gx3d' -delete
cpps=$(find ../gearoenix/ -iname *.cpp | xargs echo)
clear
clear
echo "Please wait, compiling ..."
clang++ -pthread -std=c++14 -Ofast -ldl -lSDL2 -lGL $cpps -o out.bin
