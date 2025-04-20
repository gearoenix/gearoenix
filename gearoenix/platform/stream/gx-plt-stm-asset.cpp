#include "gx-plt-stm-asset.hpp"
#include "../gx-plt-application.hpp"

#ifdef GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#endif

gearoenix::platform::stream::Asset::Asset() = default;

#ifdef GX_PLATFORM_ANDROID
gearoenix::platform::stream::Asset::~Asset()
{
    AAsset_close(file);
}
#else
gearoenix::platform::stream::Asset::~Asset() = default;
#endif

gearoenix::platform::stream::Asset* gearoenix::platform::stream::Asset::construct(const std::string& name)
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
    const std::string file_path = Application::get().get_base().get_arguments().get_process_directory() + file_name;
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

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::read(void* data, const stream_size_t length)
{
#ifdef GX_PLATFORM_ANDROID
    const auto result = static_cast<stream_size_t>(AAsset_read(file, data, static_cast<stream_size_t>(length)));
#elif defined(GX_USE_STD_FILE)
    file.read(static_cast<char*>(data), static_cast<std::streamsize>(length));
    auto result = static_cast<stream_size_t>(file.gcount());
#else
#error "Unexpected file interface"
#endif
    GX_ASSERT_D(result == length);
    return result;
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::write(const void*, stream_size_t)
{
    GX_UNEXPECTED; // Asset is only a readable stream
}

void gearoenix::platform::stream::Asset::flush()
{
    GX_UNEXPECTED; // Asset is only a readable stream
}

void gearoenix::platform::stream::Asset::seek(const stream_size_t offset)
{
#if defined(GX_USE_STD_FILE)
    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
#elif defined(GX_PLATFORM_ANDROID)
    AAsset_seek(file, static_cast<off_t>(offset), SEEK_SET);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::tell()
{
#if defined(GX_USE_STD_FILE)
    return static_cast<stream_size_t>(file.tellg());
#elif defined(GX_PLATFORM_ANDROID)
    return (stream_size_t)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::size()
{
#ifdef GX_USE_STD_FILE
    const auto c = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = static_cast<stream_size_t>(file.tellg());
    file.seekg(c);
    return s;
#elif defined(GX_PLATFORM_ANDROID)
    return static_cast<stream_size_t>(AAsset_getLength64(file));
#else
#error "Unexpected file interface"
#endif
}
