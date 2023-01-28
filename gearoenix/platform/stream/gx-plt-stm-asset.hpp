#ifndef GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#define GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#include "../gx-plt-build-configuration.hpp"
#include "gx-plt-stm-stream.hpp"
#include <memory>
#include <string>
#include <vector>

#if defined(GX_PLATFORM_DESKTOP) || defined(GX_PLATFORM_IOS) || defined(GX_PLATFORM_WEBASSEMBLY)
#define GX_USE_STD_FILE
#endif

#ifdef GX_USE_STD_FILE
#include <fstream>
#elif defined(GX_PLATFORM_ANDROID)
#include <android/asset_manager.h>
#else
#error "Unknown file implementation!"
#endif

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Asset final : public Stream {
private:
#ifdef GX_USE_STD_FILE
    std::ifstream file;
#elif defined(GX_PLATFORM_ANDROID)
    const platform::Application* platform_application = nullptr;
    AAsset* file = nullptr;
#else
#error "File usage is not specified!"
#endif

    Asset();

public:
    ~Asset() noexcept final;
    /// It will return null if file does not exist
    [[nodiscard]] static Asset* construct(const platform::Application& platform_application, const std::string& name) noexcept;
    [[nodiscard]] std::size_t read(void* data, std::size_t length) noexcept final;
    [[nodiscard]] std::size_t write(const void* data, std::size_t length) noexcept final;
    [[nodiscard]] std::size_t tell() noexcept final;
    [[nodiscard]] std::size_t size() noexcept final;
    void seek(std::size_t offset) noexcept final;
};
}

#endif
