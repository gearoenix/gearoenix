FROM ubuntu:26.04
LABEL authors="Thany Noroopzpour"
SHELL ["/bin/bash", "-c"]

RUN apt-get update && \
    apt-get dist-upgrade -y && \
    apt-get install -y build-essential cmake ninja-build clang-22 llvm-22 pkg-config ccache automake autoconf \
    autoconf-archive libtool perl python3 python3-venv python3-pip git curl tar zip unzip xz-utils patch \
    ca-certificates libx11-dev libxft-dev libxi-dev libxrandr-dev libxcursor-dev libxinerama-dev libxext-dev \
    libxfixes-dev libxtst-dev libxkbcommon-dev libwayland-dev wayland-protocols libdecor-0-dev libegl1-mesa-dev \
    libibus-1.0-dev libasound2-dev libpulse-dev pipewire-audio-client-libraries libdbus-1-dev libudev-dev libdrm-dev \
    libgbm-dev libssl-dev zlib1g-dev libltdl-dev perl-modules bash libsdl3-dev libunwind-dev rsync libicu-dev \
    libbz2-dev liblzma-dev libzstd-dev liblz4-dev uuid-dev libcap-dev libmount-dev meson libsystemd-dev valgrind \
    libcryptsetup-dev linux-libc-dev libqrencode-dev python3-jinja2 \
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

RUN apt-get update && \
    apt-get install -y openjdk-21-jdk-headless wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
ENV JAVA_HOME="/usr/lib/jvm/java-21-openjdk-amd64"

RUN mkdir -p /root/android-sdk/cmdline-tools && \
    wget -q https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip -O /tmp/cmdline-tools.zip && \
    unzip -q /tmp/cmdline-tools.zip -d /tmp/cmdline-tools && \
    mv /tmp/cmdline-tools/cmdline-tools /root/android-sdk/cmdline-tools/latest && \
    rm /tmp/cmdline-tools.zip
ENV ANDROID_HOME="/root/android-sdk"
ENV PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"

RUN yes | sdkmanager --licenses && \
    sdkmanager \
    "platforms;android-36" \
    "platform-tools" \
    "build-tools;36.0.0" \
    "ndk;30.0.14904198" \
    "cmake;4.1.2"
ENV ANDROID_NDK_HOME="$ANDROID_HOME/ndk/30.0.14904198"

RUN wget -q https://services.gradle.org/distributions/gradle-9.4.1-bin.zip -O /tmp/gradle.zip && \
    unzip -q /tmp/gradle.zip -d /opt && \
    rm /tmp/gradle.zip
ENV GRADLE_HOME="/opt/gradle-9.4.1"
ENV PATH="$GRADLE_HOME/bin:$PATH"

ENTRYPOINT ["tail", "-f", "/dev/null"]