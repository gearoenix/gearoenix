#!/bin/bash
mkdir -p assets && \
echo "Just a placeholder" > assets/placeholder && \
rm -rf webassembly-build && \
mkdir -p webassembly-build && \
cd webassembly-build && \
source $EMSDK/emsdk_env.sh && \
emcmake cmake -DCMAKE_BUILD_TYPE=MinRelSize -DGX_TEST_ENABLED=OFF .. && \
cmake --build . --config MinRelSize --parallel $(nproc --all) && \
python -m http.server 8080