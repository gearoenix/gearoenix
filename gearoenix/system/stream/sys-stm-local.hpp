#ifndef GEAROENIX_SYSTEM_FILE_LOCAL_HPP
#define GEAROENIX_SYSTEM_FILE_LOCAL_HPP

#include "sys-stm-stream.hpp"
#include <fstream>

namespace gearoenix {
namespace system {
    namespace stream {
        class Local : public Stream {
        private:
            std::ifstream file;

        public:
            Local(const std::string& name, bool writable = false);
            ~Local();
            core::Count read(void* data, core::Count length);
            core::Count write(const void* data, core::Count length);
            void seek(core::Count offset);
            core::Count tell();
            static bool exist(const std::string& name);
        };
    }
}
}
#endif
