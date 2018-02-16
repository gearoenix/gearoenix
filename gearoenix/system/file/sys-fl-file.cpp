#include "sys-fl-file.hpp"
#include "../sys-app.hpp"
#include "../sys-log.hpp"
#ifdef IN_IOS
#import <Foundation/Foundation.h>
//#import "../apple/sys-apl.mm"
#elif defined(IN_ANDROID)
#include <android_native_app_glue.h>
#endif

void gearoenix::system::file::File::check_endian_compatibility()
{
    unsigned int system_endian = 1;
    uint8_t resource_endian;
    read(resource_endian);
    is_endian_compatible = (resource_endian == ((uint8_t*)(&system_endian))[0]);
    //    LOGE(std::string("endian is: ") + std::to_string(is_endian_compatible));
}

gearoenix::system::file::File::File
#ifdef USE_STD_FILE
    (system::Application*, const std::string& name)
{
    std::string file_path = name;
#ifdef IN_IOS
    @autoreleasepool {
        //        NSString *f_name = [NSString stringWithCString:name.c_str() encoding:[NSString defaultCStringEncoding]];
        NSString* path = [[NSBundle mainBundle] pathForResource:@"data" ofType:@"gx3d"];
        file_path = std::string([path fileSystemRepresentation]);
    }
#endif
    file.open(file_path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        GXLOGF("Error in opening assets file.");
    }
#elif defined(IN_ANDROID)
    (system::Application* sys_app, const std::string& name)
//: sys_app(sys_app)
{
    file = AAssetManager_open(sys_app->get_android_app()->activity->assetManager,
        name.c_str(), AASSET_MODE_BUFFER);
    if (file == nullptr) {
        GXLOGF("Asset not found! " << name);
    }
#else
#error "Unimplemented!"
#endif
    check_endian_compatibility();
}

gearoenix::system::file::File::~File() {}

bool gearoenix::system::file::File::get_endian_compatibility() const
{
    return is_endian_compatible;
}

unsigned int gearoenix::system::file::File::read(void* data, size_t length)
{
#ifdef IN_ANDROID
    return static_cast<unsigned int>(AAsset_read(file, data, length));
#elif defined(USE_STD_FILE)
    file.read(reinterpret_cast<char*>(data), length);
    unsigned int result = static_cast<unsigned int>(file.gcount());
#ifdef DEBUG_MODE
    if (result != (unsigned int)length)
        UNEXPECTED;
#endif
    return result;
#else
#error "Error not implemented yet!"
#endif
}

void gearoenix::system::file::File::seek(unsigned int offset)
{
#if defined(USE_STD_FILE)
    file.seekg(offset, std::ios::beg);
#elif defined(IN_ANDROID)
    AAsset_seek(file, offset, SEEK_SET);
#else
#error "Error not implemented yet!"
#endif
}

unsigned int gearoenix::system::file::File::tell()
{
#if defined(USE_STD_FILE)
    return (unsigned int)file.tellg();
#elif defined(IN_ANDROID)
    return (unsigned int)AAsset_seek(file, 0, SEEK_CUR);
#else
#error "Error not implemented yet!"
#endif
}
