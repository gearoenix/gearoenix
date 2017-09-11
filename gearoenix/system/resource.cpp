#include "resource.hpp"
#include "log.hpp"
#ifdef IN_ANDROID
#include <android/asset_manager_jni.h>
#endif
gearoenix::nufrag::system::Resource::Resource(
#ifdef IN_ANDROID
        AAsset *file): native_file(file) {
#elif defined(IN_LINUX) || defined(IN_WINDOWS)
        const std::string &name): native_file(name, std::ios::in | std::ios::binary) {
    if(!native_file.is_open()) {
        LOGF("Error file is not open!");
    }
#else
#error "Error not implemented yet!"
#endif
}

unsigned int gearoenix::nufrag::system::Resource::read(void *data, size_t length) {
#ifdef IN_ANDROID
    return static_cast<unsigned int>(AAsset_read(native_file, data, length));
#elif defined(IN_LINUX) || defined(IN_WINDOWS)
    native_file.read(reinterpret_cast<char *>(data), length);
    return static_cast<unsigned int>(native_file.gcount());
#else
#error "Error not implemented yet!"
#endif
}

unsigned int gearoenix::nufrag::system::Resource::size() const {
#ifdef IN_ANDROID
    return static_cast<unsigned int>(AAsset_getLength(native_file));
#elif defined(IN_LINUX) || defined(IN_WINDOWS)
    auto nc_this = const_cast<Resource *>(this);
    auto cur_pos = nc_this->native_file.tellg();
    nc_this->native_file.seekg(0, std::ios::end);
    unsigned int result = static_cast<unsigned int>(nc_this->native_file.tellg());
    nc_this->native_file.seekg(cur_pos, std::ios::beg);
    return result;
#else
#error "Error not implemented yet!"
#endif
}
