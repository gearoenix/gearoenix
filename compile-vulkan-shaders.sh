#!/bin/bash
cfg="-V --target-env spirv1.5"
cfg="$cfg -g"
#cfg="$cfg -O3"
src_dir="gearoenix/vulkan/shader/glsl"
out_dir="../assets/spirv"

glslangValidator $cfg $src_dir/gx-vk-shd-camera.rgen      -o $out_dir/gx-vk-shd-camera.rgen.spv      &
glslangValidator $cfg $src_dir/gx-vk-shd-pbr.rchit        -o $out_dir/gx-vk-shd-pbr.rchit.spv        &
glslangValidator $cfg $src_dir/gx-vk-shd-pbr-sky.rmiss    -o $out_dir/gx-vk-shd-pbr-sky.rmiss.spv    &
glslangValidator $cfg $src_dir/gx-vk-shd-pbr-shadow.rmiss -o $out_dir/gx-vk-shd-pbr-shadow.rmiss.spv &
