#ifndef GEAROENIX_NUFRAG_SYSTEM_RESOURCE_HPP
#define GEAROENIX_NUFRAG_SYSTEM_RESOURCE_HPP
#include "../core/build-configuration.hpp"
#include <vector>
#ifdef IN_ANDROID
struct AAsset;
#elif defined(IN_LINUX) || defined(IN_WINDOWS)
#include <fstream>
#else
#error "Error not implemented yet!"
#endif
namespace gearoenix {
namespace nufrag {
namespace system {
class Resource {
private:
#ifdef IN_ANDROID
    AAsset *native_file;
#elif defined(IN_LINUX) || defined(IN_WINDOWS)
    std::fstream native_file;
#else
#error "Error not implemented yet!"
#endif
public:
    Resource(
        #ifdef IN_ANDROID
            AAsset *file
        #elif defined(IN_LINUX) || defined(IN_WINDOWS)
            const std::string &name
        #else
        #error "Error not implemented yet!"
        #endif
            );
    unsigned int read(void *data, size_t length);
    unsigned int size() const;
    template <typename T>
    unsigned int read(std::vector<T> &data) {
        return read(data.data(), data.size() * sizeof(T));
    }
};
}
}
}
#endif
