FROM ubuntu:25.10
LABEL authors="Thany Noroopzpour"
SHELL ["/bin/bash", "-c"]

RUN apt-get update || true
RUN apt-get dist-upgrade -y
RUN apt-get install build-essential automake autoconf libtool perl-modules perl python3 ninja-build libboost-dev zip \
    unzip curl tar xz-utils bash git cmake clang-20 libsdl3-dev libdrm-dev libgbm-dev libltdl-dev libunwind-dev \
    pkg-config python3-pip ca-certificates patch rsync ccache libxi-dev libxrandr-dev libxcursor-dev libxinerama-dev \
    libxext-dev libxfixes-dev libxkbcommon-dev libwayland-dev wayland-protocols libdecor-0-dev libdbus-1-dev \
    libudev-dev libasound2-dev pipewire-audio-client-libraries libpulse-dev libicu-dev libbz2-dev liblzma-dev \
    libssl-dev libzstd-dev liblz4-dev zlib1g-dev uuid-dev libcap-dev libmount-dev meson libsystemd-dev valgrind \
    libcryptsetup-dev linux-libc-dev linux-libc-dev libqrencode-dev python3-jinja2 -y

ENV CC="/usr/bin/clang-20"
ENV CXX="/usr/bin/clang++-20"
ENV CMAKE_BUILD_PARALLEL_LEVEL=32
ENV LLVM_PROFILE_FILE=gearoenix.profraw

RUN curl -L https://vulkan.lunarg.com/sdk/download/latest/linux/vulkan-sdk.tar.gz --output /root/vulkan-sdk.tar.gz && \
    mkdir -p /root/vulkan-sdk && \
    tar -xf /root/vulkan-sdk.tar.gz -C /root/vulkan-sdk --strip-components=1 && \
    rm /root/vulkan-sdk.tar.gz
ENV VULKAN_SDK="/root/vulkan-sdk/x86_64"
ENV PATH="$VULKAN_SDK/bin:$PATH"

RUN git clone https://github.com/emscripten-core/emsdk.git /root/emsdk && \
    cd /root/emsdk && \
    ./emsdk install latest && \
    ./emsdk activate latest
ENV EMSDK="/root/emsdk"

RUN git clone https://github.com/microsoft/vcpkg.git /root/vcpkg && \
    cd /root/vcpkg &&  \
    ./bootstrap-vcpkg.sh
ENV VCPKG_ROOT="/root/vcpkg"

ENTRYPOINT ["tail", "-f", "/dev/null"]