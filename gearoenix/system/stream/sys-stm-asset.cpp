#include "sys-stm-asset.hpp"
#include "../sys-app.hpp"
#include "../sys-log.hpp"
#ifdef GX_IN_IOS
#import <Foundation/Foundation.h>
//#import "../apple/sys-apl.mm"
#elif defined(GX_IN_ANDROID)
#include <android_native_app_glue.h>
#endif

void gearoenix::system::stream::Asset::check_endian_compatibility()
{
    unsigned int system_endian = 1;
    uint8_t resource_endian;
    Stream::read(resource_endian);
    is_endian_compatible = (resource_endian == ((uint8_t*)(&system_endian))[0]);
}

gearoenix::system::stream::Asset::Asset
#ifdef GX_USE_STD_FILE
    (const std::shared_ptr<system::Application>&, const std::string& name)
{
    std::string file_path = name;
#ifdef GX_IN_IOS
    @autoreleasepool {
        //        NSString *f_name = [NSString stringWithCString:name.c_str() encoding:[NSString defaultCStringEncoding]];
        NSString* path = [[NSBundle mainBundle] pathForResource:@"data" ofType:@"gx3d"];
        file_path = std::string([path fileSystemRepresentation]);
    }
#endif
    file.open(file_path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        GXLOGF("Error in opening assets file: " << name);
    }
#elif defined(GX_IN_ANDROID)
    (const std::shared_ptr<system::Application>& sys_app, const std::string& name)
//: sys_app(sys_app)
{
    file = AAssetManager_open(sys_app->get_android_app()->activity->assetManager,
        name.c_str(), AASSET_MODE_BUFFER);
    if (file == nullptr) {
        GXLOGF("Asset not found! " << name);
    }
#else
#error "Unexpected file interface!"
#endif
    check_endian_compatibility();
}

gearoenix::system::stream::Asset::~Asset()
{
    GXTODO; //android asset free check
}

gearoenix::core::Count gearoenix::system::stream::Asset::read(void* data, core::Count length)
{
#ifdef GX_IN_ANDROID
    core::Count result = static_cast<core::Count>(AAsset_read(file, data, length));
#elif defined(GX_USE_STD_FILE)
    file.read(static_cast<char*>(data), length);
    core::Count result = static_cast<core::Count>(file.gcount());
#else
#error "Unexpected file interface"
#endif
#ifdef GX_DEBUG_MODE
    if (result != length)
        GXUNEXPECTED;
#endif
    return result;
}

gearoenix::core::Count gearoenix::system::stream::Asset::write(const void*, core::Count)
{
    GXUNEXPECTED;
}

void gearoenix::system::stream::Asset::seek(core::Count offset)
{
#if defined(GX_USE_STD_FILE)
    file.seekg(offset, std::ios::beg);
#elif defined(GX_IN_ANDROID)
    AAsset_seek(file, offset, SEEK_SET);
#else
#error "Unexpected file interface"
#endif
}

gearoenix::core::Count gearoenix::system::stream::Asset::tell()
{
#if defined(GX_USE_STD_FILE)
    return (core::Count)file.tellg();
#elif defined(GX_IN_ANDROID)
    return (core::Count)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Unexpected file interface"
#endif
}
