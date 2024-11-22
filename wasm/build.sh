#!/bin/bash
mkdir -p build && \
cd build && \
source "$EMSDK/emsdk_env.sh" && \
emcmake cmake -DCMAKE_BUILD_TYPE=MinRelSize -DGX_TEST_ENABLED=OFF ../.. && \
cmake --build . --config MinRelSize --parallel $(nproc --all) && \
../run-server.py