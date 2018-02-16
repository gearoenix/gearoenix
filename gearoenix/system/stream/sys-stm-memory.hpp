#ifndef GEAROENIX_SYSTEM_FILE_MEMORY_HPP
#define GEAROENIX_SYSTEM_FILE_MEMORY_HPP

#include "sys-stm-stream.hpp"
#include <vector>

namespace gearoenix {
namespace system {
    namespace stream {
        class Memory : public Stream {
        private:
            std::vector<unsigned char> data;

        public:
            Memory();
            ~Memory();
            unsigned int read(void* data, size_t length);
            unsigned int write(const void* data, size_t length);
            void seek(unsigned int offset);
            unsigned int tell();
        };
    }
}
}
#endif
