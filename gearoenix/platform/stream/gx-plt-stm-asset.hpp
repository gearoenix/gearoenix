#ifndef GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#define GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#include "../gx-plt-build-configuration.hpp"
#include "gx-plt-stm-stream.hpp"
#include <memory>
#include <string>
#include <vector>

#if defined(GX_PLATFORM_DESKTOP) || defined(GX_PLATFORM_IOS) || defined(GX_PLATFORM_WEB)
#define GX_USE_STD_FILE
#endif

#ifdef GX_USE_STD_FILE
#include <fstream>
#elif defined(GX_IN_ANDROID)
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
#elif defined(GX_IN_ANDROID)
    platform::Application* sys_app;
    AAsset* file = nullptr;
#else
#error "File usage is not specified!"
#endif
    void check_endian_compatibility() noexcept;

    Asset();

public:
    ~Asset() noexcept final;
    /// It will return null if file does not exist
    [[nodiscard]] static Asset* construct(platform::Application* plt_app, const std::string& name, bool relative_path = true) noexcept;
    [[nodiscard]] core::Count read(void* data, core::Count length) noexcept final;
    [[nodiscard]] core::Count write(const void* data, core::Count length) noexcept final;
    void seek(core::Count offset) noexcept final;
    [[nodiscard]] core::Count tell() noexcept final;
    [[nodiscard]] core::Count size() noexcept final;
};
}

#endif
