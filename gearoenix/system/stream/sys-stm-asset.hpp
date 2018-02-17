#ifndef GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#define GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#include "../../core/cr-build-configuration.hpp"
#include "sys-stm-stream.hpp"
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
#error "Not implemented yet!"
#endif
namespace gearoenix {
namespace system {
    class Application;
    namespace stream {
        class Asset : public Stream {
        private:
#ifdef USE_STD_FILE
            std::ifstream file;
#elif defined(IN_ANDROID)
            //system::Application* sys_app;
            AAsset* file;
#else
#error "Not implemented yet!"
#endif
            bool is_endian_compatible = true;
            void check_endian_compatibility();
            void built_in_type_read(void* data, core::Count length);

        public:
            Asset(system::Application* sys_app, const std::string& name);
            ~Asset();
            bool get_endian_compatibility() const;
            core::Count read(void* data, core::Count length);
            core::Count write(const void* data, core::Count length);
            void seek(core::Count offset);
            core::Count tell();
        };
    }
}
}
#endif
