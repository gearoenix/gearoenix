#pragma once
#include "../gx-plt-build-configuration.hpp"
#include "gx-plt-stm-stream.hpp"
#include <string>
#include <vector>

#if defined(GX_PLATFORM_DESKTOP) || defined(GX_PLATFORM_IOS) || defined(GX_PLATFORM_WEBASSEMBLY)
#define GX_USE_STD_FILE true
#endif

#if GX_PLATFORM_INTERFACE_SDL
#include <SDL3/SDL_iostream.h>
#elif GX_USE_STD_FILE
#include <fstream>
#elif GX_PLATFORM_ANDROID
#include <android/asset_manager.h>
#else
#error "Unknown file implementation!"
#endif

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Asset final : Stream {
private:
#if GX_PLATFORM_INTERFACE_SDL
    SDL_IOStream* file = nullptr;
#elif GX_USE_STD_FILE
    std::ifstream file;
#elif GX_PLATFORM_ANDROID
    AAsset* file = nullptr;
#else
#error "File usage is not specified!"
#endif

    Asset();

public:
    ~Asset() override;
    /// It will return null if the file does not exist.
    [[nodiscard]] static Asset* construct(const std::string& name);
    [[nodiscard]] stream_size_t read(void* data, stream_size_t length) override;
    [[nodiscard]] stream_size_t write(const void* data, stream_size_t length) override;
    [[nodiscard]] stream_size_t tell() override;
    [[nodiscard]] stream_size_t size() override;
    void flush() override;
    void seek(stream_size_t offset) override;
};
}
