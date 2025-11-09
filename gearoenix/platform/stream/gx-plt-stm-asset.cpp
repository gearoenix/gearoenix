#include "gx-plt-stm-asset.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-arguments.hpp"

#if GX_PLATFORM_INTERFACE_SDL
#include <SDL3/SDL_system.h>
#elif GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif GX_PLATFORM_ANDROID
#include <android_native_app_glue.h>
#endif

gearoenix::platform::stream::Asset::Asset() = default;

gearoenix::platform::stream::Asset::~Asset()
#if GX_PLATFORM_INTERFACE_SDL
{
    [[maybe_unused]] const auto ok = SDL_CloseIO(file);
    GX_ASSERT_D(ok);
}
#elif GX_PLATFORM_ANDROID
{
    AAsset_close(file);
}
#else
 = default;
#endif

gearoenix::platform::stream::Asset* gearoenix::platform::stream::Asset::construct(const std::string& name)
{
#if GX_PLATFORM_ANDROID
    const std::string& file_name = name;
#else
    const std::string file_name = "assets/" + name;
#endif

    auto* const asset = new Asset();
#if GX_PLATFORM_INTERFACE_SDL
    asset->file = SDL_IOFromFile(file_name.c_str(), "rb");
    if(!asset->file) {
        GX_LOG_D("Can not find/open assets file: " << file_name << " error: " << SDL_GetError());
        delete asset;
        return nullptr;
    }
#elif GX_USE_STD_FILE
#if GX_PLATFORM_IOS
    std::string file_path;
    @autoreleasepool {
        NSString* path = [[NSBundle mainBundle] resourcePath];
        file_path = core::String::join_path(path, file_name);
    }
#else
    const std::string file_path = Arguments::get().get_process_directory() + file_name;
#endif
    asset->file.open(file_path, std::ios::binary | std::ios::in);
    if (!asset->file.is_open()) {
        GX_LOG_D("Can not find/open assets file: " << name);
        delete asset;
        return nullptr;
    }
#elif GX_PLATFORM_ANDROID
    asset->file = AAssetManager_open(SDL_GetAndroidActivity().activity->assetManager, name.c_str(), AASSET_MODE_BUFFER);
    GX_ASSERT_D(asset->file);
#else
#error "Unexpected file interface!"
#endif
    return asset;
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::read(void* data, const stream_size_t length)
{
#if GX_PLATFORM_INTERFACE_SDL
    const auto result = SDL_ReadIO(file, data, static_cast<int>(length));
#elif GX_PLATFORM_ANDROID
    const auto result = static_cast<stream_size_t>(AAsset_read(file, data, static_cast<stream_size_t>(length)));
#elif GX_USE_STD_FILE
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
#if GX_PLATFORM_INTERFACE_SDL
    SDL_SeekIO(file, static_cast<Sint64>(offset), SDL_IO_SEEK_SET);
#elif GX_USE_STD_FILE
    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
#elif GX_PLATFORM_ANDROID
    AAsset_seek(file, static_cast<off_t>(offset), SEEK_SET);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::tell()
{
#if GX_PLATFORM_INTERFACE_SDL
    return SDL_TellIO(file);
#elif GX_USE_STD_FILE
    return static_cast<stream_size_t>(file.tellg());
#elif GX_PLATFORM_ANDROID
    return (stream_size_t)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Asset::size()
{
#if GX_PLATFORM_INTERFACE_SDL
    return SDL_GetIOSize(file);
#elif GX_USE_STD_FILE
    const auto c = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = static_cast<stream_size_t>(file.tellg());
    file.seekg(c);
    return s;
#elif GX_PLATFORM_ANDROID
    return static_cast<stream_size_t>(AAsset_getLength64(file));
#else
#error "Unexpected file interface"
#endif
}
