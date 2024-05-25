FROM ubuntu:24.04
LABEL authors="Thany Noroopzpourthanyabady"
SHELL ["/bin/bash", "-c"]

RUN apt-get update || true
RUN apt-get dist-upgrade -y
RUN apt-get install curl tar xz-utils bash git cmake clang-18 libsdl2-dev libdrm-dev libgbm-dev -y
ENV CC="/usr/bin/clang-18"
ENV CXX="/usr/bin/clang++-18"
ENV CMAKE_BUILD_PARALLEL_LEVEL=32
ENV LLVM_PROFILE_FILE="'gearoenix.profraw'"
RUN curl -L https://vulkan.lunarg.com/sdk/download/latest/linux/vulkan-sdk.tar.gz --output /root/vulkan-sdk.tar.gz
RUN mkdir -p /root/vulkan-sdk
RUN tar -xf /root/vulkan-sdk.tar.gz -C /root/vulkan-sdk --strip-components=1
RUN rm /root/vulkan-sdk.tar.gz
ENV VULKAN_SDK="/root/vulkan-sdk"
ENV PATH="$VULKAN_SDK/bin:${PATH}"
RUN git clone https://github.com/emscripten-core/emsdk.git /root/emsdk
RUN /root/emsdk/emsdk install latest
RUN /root/emsdk/emsdk activate latest

ENTRYPOINT ["tail", "-f", "/dev/null"]