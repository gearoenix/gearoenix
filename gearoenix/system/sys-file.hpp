#ifndef GEAROENIX_SYSTEM_FILE_HPP
#define GEAROENIX_SYSTEM_FILE_HPP

#include "../core/cr-build-configuration.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

#if defined(IN_LINUX) || defined(IN_WINDOWS)
#include <fstream>
#elif defined(IN_ANDROID)

#include <android/asset_manager.h>

#else
#error "Unimplemented yet!"
#endif
namespace gearoenix {
namespace system {
    class Application;
    class File {
    private:
#if defined(IN_LINUX) || defined(IN_WINDOWS)
        system::Application* sys_app;
        std::ifstream file;
#elif defined(IN_ANDROID)
        AAsset* file;
#else
#error "Unimplemented yet!"
#endif
        bool is_endian_compatible = true;

        template <typename T>
        void change_endianess(T* data)
        {
            uint8_t* c_data = reinterpret_cast<uint8_t*>(data);
            for (int i = 0, j = sizeof(T) - 1; i < j; ++i, --j) {
                uint8_t tmp = c_data[i];
                c_data[i] = c_data[j];
                c_data[j] = tmp;
            }
        }

        template <typename T>
        void correct_endianess(T* data)
        {
            if (is_endian_compatible || sizeof(T) == 1) {
                return;
            } else {
                change_endianess(data);
            }
        }

        void check_endian_compatibility();

    public:
        File(system::Application* sys_app);
        ~File();
        bool get_endian_compatibility() const;
        unsigned int read(void* data, size_t length);
        void seek(unsigned int offset);

        template <typename T>
        unsigned int read(std::vector<T>& data)
        {
            unsigned int result = read(data.data(), data.size() * sizeof(T));
            unsigned int count = result / sizeof(T);
            if (is_endian_compatible)
                return count;
            for (unsigned int i = 0; i < count; ++i) {
                change_endianess(&(data[i]));
            }
            return count;
        }

        template <typename T>
        void read(T& data)
        {
            read(&data, sizeof(T));
            correct_endianess(&data);
        }
    };
}
}
#endif
