FROM ubuntu:26.04
LABEL authors="Thany Noroopzpour"
SHELL ["/bin/bash", "-c"]

RUN apt-get update && \
    apt-get dist-upgrade -y && \
    apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build clang-22 llvm-22 pkg-config ccache \
    automake autoconf libtool perl python3 \
    git curl tar zip unzip xz-utils patch ca-certificates \
    libxi-dev libxrandr-dev libxcursor-dev libxinerama-dev \
    libxext-dev libxfixes-dev \
    libxkbcommon-dev libwayland-dev wayland-protocols libdecor-0-dev \
    libasound2-dev libpulse-dev pipewire-audio-client-libraries \
    libdbus-1-dev libudev-dev libdrm-dev libgbm-dev \
    libssl-dev zlib1g-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV CC="/usr/bin/clang-22"
ENV CXX="/usr/bin/clang++-22"
ENV CMAKE_BUILD_PARALLEL_LEVEL=32
ENV LLVM_PROFILE_FILE=gearoenix.profraw

RUN curl -L https://vulkan.lunarg.com/sdk/download/latest/linux/vulkan-sdk.tar.gz --output /tmp/vulkan-sdk.tar.gz && \
    mkdir -p /root/vulkan-sdk && \
    tar -xf /tmp/vulkan-sdk.tar.gz -C /root/vulkan-sdk --strip-components=1 && \
    rm /tmp/vulkan-sdk.tar.gz
ENV VULKAN_SDK="/root/vulkan-sdk/x86_64"
ENV PATH="$VULKAN_SDK/bin:$PATH"

RUN git clone --depth 1 https://github.com/emscripten-core/emsdk.git /root/emsdk && \
    cd /root/emsdk && \
    ./emsdk install latest && \
    ./emsdk activate latest
ENV EMSDK="/root/emsdk"

RUN git clone https://github.com/microsoft/vcpkg.git /root/vcpkg && \
    cd /root/vcpkg && \
    ./bootstrap-vcpkg.sh
ENV VCPKG_ROOT="/root/vcpkg"

ENTRYPOINT ["tail", "-f", "/dev/null"]