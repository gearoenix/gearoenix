#ifndef GEAROENIX_SYSTEM_FILE_HPP
#define GEAROENIX_SYSTEM_FILE_HPP

#include "../core/cr-build-configuration.hpp"
#include "../core/cr-types.hpp"
#include "../system/sys-log.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

#if defined(IN_DESKTOP) || defined(IN_IOS) || defined(IN_WEB)
#define USE_STD_FILE
#endif

#ifdef USE_STD_FILE
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
#ifdef USE_STD_FILE
        std::ifstream file;
#elif defined(IN_ANDROID)
        //system::Application* sys_app;
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
        File(system::Application* sys_app, const std::string& name);
        ~File();
        bool get_endian_compatibility() const;
        unsigned int read(void* data, size_t length);
        void seek(unsigned int offset);
        unsigned int tell();

        template <typename T>
        void read(std::vector<T>& data)
        {
            core::Count c;
            read(c);
            data.resize((size_t)c);
            size_t s = (size_t)(c * sizeof(T));
#ifdef DEBUG_MODE
            if (read(reinterpret_cast<void*>(data.data()), s) != s) {
                UNEXPECTED;
            }
#else
            read(reinterpret_cast<void*>(data.data()), s);
#endif
            if (is_endian_compatible)
                return;
            for (unsigned int i = 0; i < c; ++i) {
                change_endianess(&(data[i]));
            }
        }

        template <typename T>
        void read(T& data)
        {
#ifdef DEBUG_MODE
            if (sizeof(T) != read(&data, sizeof(T))) {
                UNEXPECTED;
            }
#else
            read(&data, sizeof(T));
#endif
            correct_endianess(&data);
        }

        template <typename T>
        T read()
        {
            T data;
            read(data);
            return data;
        }

        bool read_bool()
        {
            uint8_t data;
#ifdef DEBUG_MODE
            if (1 != read(&data, 1)) {
                GXLOGF("Unexpected");
            }
#else
            read(&data, 1);
#endif
            return data != 0;
        }
    };
}
}
#endif
