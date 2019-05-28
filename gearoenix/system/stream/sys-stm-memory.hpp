#ifndef GEAROENIX_SYSTEM_STREAM_MEMORY_HPP
#define GEAROENIX_SYSTEM_STREAM_MEMORY_HPP

#include "sys-stm-stream.hpp"
#include <vector>

namespace gearoenix {
namespace system {
    namespace stream {
        class Memory : public Stream {
        private:
            std::vector<unsigned char> mem_data;
            core::Count index = 0;

        public:
            Memory() noexcept;
            ~Memory() noexcept final;
            core::Count read(void* d, core::Count length) noexcept;
            core::Count write(const void* d, core::Count length) noexcept;
            void seek(core::Count offset) noexcept;
            core::Count tell() noexcept;
        };
    }
}
}
#endif
