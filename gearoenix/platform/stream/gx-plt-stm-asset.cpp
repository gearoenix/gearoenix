#include "gx-plt-stm-asset.hpp"
#include "../gx-plt-application.hpp"

#ifdef GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#endif

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
    const platform::Application& platform_application, const std::string& name) noexcept
{
    const std::string file_name = "assets/" + name;
    auto* const asset = new Asset();
#ifdef GX_USE_STD_FILE
#ifdef GX_PLATFORM_IOS
    std::string file_path;
    @autoreleasepool {
        NSString* path = [[NSBundle mainBundle] resourcePath];
        file_path = core::String::join_path(path, file_name);
    }
#else
    const std::string file_path = platform_application.get_base().get_arguments().get_process_directory() + file_name;
#endif
    asset->file.open(file_path, std::ios::binary | std::ios::in);
    if (!asset->file.is_open()) {
        GX_LOG_D("Can not find/open assets file: " << name);
        delete asset;
        return nullptr;
    }
#elif defined(GX_PLATFORM_ANDROID)
    asset->platform_application = &platform_application;
    asset->file = AAssetManager_open(platform_application.get_android_application()->activity->assetManager, name.c_str(), AASSET_MODE_BUFFER);
    if (asset->file == nullptr) {
        GX_LOG_D("Asset not found! " << name);
        return nullptr;
    }
#else
#error "Unexpected file interface!"
#endif
    return asset;
}

std::size_t gearoenix::platform::stream::Asset::read(void* data, const std::size_t length) noexcept
{
#ifdef GX_PLATFORM_ANDROID
    const auto result = static_cast<std::size_t>(AAsset_read(file, data, static_cast<std::size_t>(length)));
#elif defined(GX_USE_STD_FILE)
    file.read(static_cast<char*>(data), static_cast<std::streamsize>(length));
    auto result = static_cast<std::size_t>(file.gcount());
#else
#error "Unexpected file interface"
#endif
#ifdef GX_DEBUG_MODE
    if (result != length)
        GX_UNEXPECTED;
#endif
    return result;
}

std::size_t gearoenix::platform::stream::Asset::write(const void*, std::size_t) noexcept
{
    GX_UNEXPECTED;
}

void gearoenix::platform::stream::Asset::flush() noexcept
{
    GX_UNEXPECTED; // Asset is only a reader stream
}

void gearoenix::platform::stream::Asset::seek(std::size_t offset) noexcept
{
#if defined(GX_USE_STD_FILE)
    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
#elif defined(GX_PLATFORM_ANDROID)
    AAsset_seek(file, static_cast<off_t>(offset), SEEK_SET);
#else
#error "Unexpected file interface"
#endif
}

std::size_t gearoenix::platform::stream::Asset::tell() noexcept
{
#if defined(GX_USE_STD_FILE)
    return (std::size_t)file.tellg();
#elif defined(GX_PLATFORM_ANDROID)
    return (std::size_t)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Unexpected file interface"
#endif
}

std::size_t gearoenix::platform::stream::Asset::size() noexcept
{
#ifdef GX_USE_STD_FILE
    const auto c = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = static_cast<std::size_t>(file.tellg());
    file.seekg(c);
    return s;
#elif defined(GX_PLATFORM_ANDROID)
    return static_cast<std::size_t>(AAsset_getLength64(file));
#else
#error "Unexpected file interface"
#endif
}
