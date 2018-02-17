#ifndef GEAROENIX_SYSTEM_STREAM_MEMORY_HPP
#define GEAROENIX_SYSTEM_STREAM_MEMORY_HPP

#include "sys-stm-stream.hpp"
#include <vector>

namespace gearoenix {
namespace system {
    namespace stream {
        class Memory : public Stream {
        private:
            std::vector<unsigned char> data;
            core::Count index = 0;

        public:
            Memory();
            ~Memory();
            core::Count read(void* data, core::Count length);
            core::Count write(const void* data, core::Count length);
            void seek(core::Count offset);
            core::Count tell();
        };
    }
}
}
#endif
