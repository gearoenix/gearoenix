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
            Memory();
            ~Memory();
            core::Count read(void* d, core::Count length);
            core::Count write(const void* d, core::Count length);
            void seek(core::Count offset);
            core::Count tell();
        };
    }
}
}
#endif
