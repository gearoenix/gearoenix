#!/bin/bash
mkdir -p wasm-build
mkdir -p assets
cd wasm-build || exit
source "$EMSDK"/emsdk_env.sh
emcmake cmake -DCMAKE_BUILD_TYPE=MinRelSize -DGX_ENABLE_DEMO_WORLD_WIDE_RACING=ON ..
cmake --build . --config MinRelSize --parallel 8  # "$(nproc --all)"