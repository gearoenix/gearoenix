#ifndef GEAROENIX_SYSTEM_STREAM_LOCAL_HPP
#define GEAROENIX_SYSTEM_STREAM_LOCAL_HPP

#include "sys-stm-stream.hpp"
#include <fstream>

namespace gearoenix {
namespace system {
    namespace stream {
        class Local : public Stream {
        private:
            std::fstream file;

        public:
            Local(const std::string& name, bool writable = false) noexcept;
            ~Local() noexcept final;
            core::Count read(void* data, core::Count length) noexcept;
            core::Count write(const void* data, core::Count length) noexcept;
            void seek(core::Count offset) noexcept;
            core::Count tell() noexcept;
            static bool exist(const std::string& name) noexcept;
        };
    }
}
}
#endif
