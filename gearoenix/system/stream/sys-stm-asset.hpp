#ifndef GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#define GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#include "../../core/cr-build-configuration.hpp"
#include "sys-stm-stream.hpp"
#include <memory>
#include <string>
#include <vector>

#if defined(GX_IN_DESKTOP) || defined(GX_IN_IOS) || defined(GX_IN_WEB)
#define GX_USE_STD_FILE
#endif

#ifdef GX_USE_STD_FILE
#include <fstream>
#elif defined(GX_IN_ANDROID)
#include <android/asset_manager.h>
#else
#error "Unknown file implementation!"
#endif

namespace gearoenix::system {
class Application;
}

namespace gearoenix::system::stream {
class Asset : public Stream {
private:
#ifdef GX_USE_STD_FILE
    std::ifstream file;
#elif defined(GX_IN_ANDROID)
    system::Application* sys_app;
    AAsset* file = nullptr;
#else
#error "File usage is not specified!"
#endif
    void check_endian_compatibility() noexcept;

    Asset() noexcept;

public:
    ~Asset() noexcept final;
    /// It will return null it file does not exist
    [[nodiscard]] static Asset* construct(system::Application* sys_app, const std::string& name) noexcept;
    [[nodiscard]] core::Count read(void* data, core::Count length) noexcept final;
    [[nodiscard]] core::Count write(const void* data, core::Count length) noexcept final;
    void seek(core::Count offset) noexcept final;
    [[nodiscard]] core::Count tell() noexcept final;
    [[nodiscard]] core::Count size() noexcept final;
};
}

#endif
