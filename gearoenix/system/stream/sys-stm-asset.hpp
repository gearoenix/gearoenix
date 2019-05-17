#ifndef GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#define GEAROENIX_SYSTEM_STREAM_ASSET_HPP
#include "../../core/cr-build-configuration.hpp"
#include "sys-stm-stream.hpp"
#include <memory>
#include <string>
#include <vector>

#if defined(GX_IN_DESKTOP) || defined(GX_IN_IOS) || defined(GX_IN_WEB)
#define GX_USE_STD_FILE
#endif

#ifdef GX_USE_STD_FILE
#include <fstream>
#elif defined(GX_IN_ANDROID)
#include <android/asset_manager.h>
#else
#error "Unknown file implementation!"
#endif
namespace gearoenix {
namespace system {
    class Application;
    namespace stream {
        class Asset : public Stream {
        private:
#ifdef GX_USE_STD_FILE
            std::ifstream file;
#elif defined(GX_IN_ANDROID)
            //system::Application* sys_app;
            AAsset* file;
#else
#error "File usage is not specified!"
#endif
            void check_endian_compatibility();

            Asset();

        public:
            ~Asset();
            /// It will return null it file does not exist
            static std::shared_ptr<Asset> construct(const std::shared_ptr<system::Application>& sys_app, const std::string& name) noexcept;
            core::Count read(void* data, core::Count length);
            core::Count write(const void* data, core::Count length);
            void seek(core::Count offset);
            core::Count tell();
        };
    }
}
}
#endif
