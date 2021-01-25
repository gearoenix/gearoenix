#include "gx-plt-stm-asset.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-log.hpp"

#ifdef GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#endif

void gearoenix::platform::stream::Asset::check_endian_compatibility() noexcept
{
    unsigned int system_endian = 1;
    uint8_t resource_endian;
    Stream::read(resource_endian);
    endian_compatibility = (resource_endian == ((uint8_t*)(&system_endian))[0]);
}

gearoenix::platform::stream::Asset::Asset() = default;

#ifdef GX_PLATFORM_ANDROID
gearoenix::platform::stream::Asset::~Asset() noexcept
{
    AAsset_close(file);
}
#else
gearoenix::platform::stream::Asset::~Asset() noexcept = default;
#endif

gearoenix::platform::stream::Asset* gearoenix::platform::stream::Asset::construct(
    platform::Application* const plt_app, const std::string& name, const bool relative_path) noexcept
{
    auto* const asset = new Asset();
#ifdef GX_USE_STD_FILE
#ifdef GX_PLATFORM_IOS
    std::string file_path;
    @autoreleasepool {
        NSString* path = [[NSBundle mainBundle] resourcePath];
        file_path = core::String::join_path(path, name);
        GXLOGD("iOS generated file path is: " << file_path)
    }
#else
    const std::string file_path = relative_path ? plt_app->get_base().get_arguments().get_process_directory() + name : name;
#endif
    asset->file.open(file_path, std::ios::binary | std::ios::in);
    if (!asset->file.is_open()) {
        GX_LOG_D("Can not find/open assets file: " << name)
        delete asset;
        return nullptr;
    }
#elif defined(GX_PLATFORM_ANDROID)
    asset->platform_application = platform_application;
    asset->file = AAssetManager_open(platform_application->get_android_application()->activity->assetManager, name.c_str(), AASSET_MODE_BUFFER);
    if (asset->file == nullptr) {
        GXLOGD("Asset not found! " << name)
        return nullptr;
    }
#else
#error "Unexpected file interface!"
#endif
    asset->check_endian_compatibility();
    (void)plt_app;
    (void)relative_path;
    return asset;
}

gearoenix::core::Count gearoenix::platform::stream::Asset::read(void* data, core::Count length) noexcept
{
#ifdef GX_PLATFORM_ANDROID
    const auto result = static_cast<core::Count>(AAsset_read(file, data, static_cast<std::size_t>(length)));
#elif defined(GX_USE_STD_FILE)
    file.read(static_cast<char*>(data), length);
    auto result = static_cast<core::Count>(file.gcount());
#else
#error "Unexpected file interface"
#endif
#ifdef GX_DEBUG_MODE
    if (result != length)
        GX_UNEXPECTED
#endif
    return result;
}

gearoenix::core::Count gearoenix::platform::stream::Asset::write(const void*, core::Count) noexcept
{
    GX_UNEXPECTED
}

void gearoenix::platform::stream::Asset::seek(core::Count offset) noexcept
{
#if defined(GX_USE_STD_FILE)
    file.seekg(offset, std::ios::beg);
#elif defined(GX_PLATFORM_ANDROID)
    AAsset_seek(file, static_cast<off_t>(offset), SEEK_SET);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::core::Count gearoenix::platform::stream::Asset::tell() noexcept
{
#if defined(GX_USE_STD_FILE)
    return (core::Count)file.tellg();
#elif defined(GX_PLATFORM_ANDROID)
    return (core::Count)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::core::Count gearoenix::platform::stream::Asset::size() noexcept
{
#ifdef GX_USE_STD_FILE
    const auto c = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = static_cast<core::Count>(file.tellg());
    file.seekg(c);
    return s;
#elif defined(GX_PLATFORM_ANDROID)
    return static_cast<core::Count>(AAsset_getLength64(file));
#else
#error "Unexpected file interface"
#endif
}
