#include "gx-sys-stm-asset.hpp"
#include "../gx-sys-application.hpp"
#include "../gx-sys-args.hpp"
#include "../gx-sys-log.hpp"

#ifdef GX_IN_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include <android_native_app_glue.h>
#endif

void gearoenix::system::stream::Asset::check_endian_compatibility() noexcept
{
    unsigned int system_endian = 1;
    uint8_t resource_endian;
    Stream::read(resource_endian);
    endian_compatibility = (resource_endian == ((uint8_t*)(&system_endian))[0]);
}

gearoenix::system::stream::Asset::Asset() = default;

#ifdef GX_IN_ANDROID
gearoenix::system::stream::Asset::~Asset() noexcept
{
    AAsset_close(file);
}
#else
gearoenix::system::stream::Asset::~Asset() noexcept = default;
#endif

gearoenix::system::stream::Asset* gearoenix::system::stream::Asset::construct(system::Application* const sys_app, const std::string& name, const bool relative_path) noexcept
{
    auto* const asset = new Asset();
#ifdef GX_USE_STD_FILE
#ifdef GX_IN_IOS
    std::string file_path;
    @autoreleasepool {
        NSString* path = [[NSBundle mainBundle] resourcePath];
        file_path = core::String::join_path(path, name);
        GXLOGD("iOS generated file path is: " << file_path)
    }
#else
    const std::string file_path = relative_path ? sys_app->get_arguments()->get_process_directory() + name : name;
#endif
    asset->file.open(file_path, std::ios::binary | std::ios::in);
    if (!asset->file.is_open()) {
        GXLOGD("Can not find/open assets file: " << name)
        delete asset;
        return nullptr;
    }
#elif defined(GX_IN_ANDROID)
    asset->sys_app = sys_app;
    asset->file = AAssetManager_open(sys_app->get_android_application()->activity->assetManager, name.c_str(), AASSET_MODE_BUFFER);
    if (asset->file == nullptr) {
        GXLOGD("Asset not found! " << name)
        return nullptr;
    }
#else
#error "Unexpected file interface!"
#endif
    asset->check_endian_compatibility();
    (void)sys_app;
    (void)relative_path;
    return asset;
}

gearoenix::core::Count gearoenix::system::stream::Asset::read(void* data, core::Count length) noexcept
{
#ifdef GX_IN_ANDROID
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

gearoenix::core::Count gearoenix::system::stream::Asset::write(const void*, core::Count) noexcept
{
    GX_UNEXPECTED
}

void gearoenix::system::stream::Asset::seek(core::Count offset) noexcept
{
#if defined(GX_USE_STD_FILE)
    file.seekg(offset, std::ios::beg);
#elif defined(GX_IN_ANDROID)
    AAsset_seek(file, static_cast<off_t>(offset), SEEK_SET);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::core::Count gearoenix::system::stream::Asset::tell() noexcept
{
#if defined(GX_USE_STD_FILE)
    return (core::Count)file.tellg();
#elif defined(GX_IN_ANDROID)
    return (core::Count)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::core::Count gearoenix::system::stream::Asset::size() noexcept
{
#ifdef GX_USE_STD_FILE
    const auto c = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = static_cast<core::Count>(file.tellg());
    file.seekg(c);
    return s;
#elif defined(GX_IN_ANDROID)
    return static_cast<core::Count>(AAsset_getLength64(file));
#else
#error "Unexpected file interface"
#endif
}
