#!/bin/bash
mkdir -p wasm-build
mkdir -p assets
echo "Just a placeholder" > assets/placeholder
cd wasm-build || exit
source "$EMSDK"/emsdk_env.sh
emcmake cmake -DCMAKE_BUILD_TYPE=MinRelSize -DGX_ENABLE_DEMO_WORLD_WIDE_RACING=ON ..
cmake --build . --config MinRelSize --parallel 4  # "$(nproc --all)"