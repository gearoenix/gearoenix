#ifndef GEAROENIX_SYSTEM_FILE_LOCAL_HPP
#define GEAROENIX_SYSTEM_FILE_LOCAL_HPP

#include "sys-fl-file.hpp"
#include <fstream>

namespace gearoenix {
namespace system {
namespace file {
    class Local: public File {
    private:
        std::ifstream file;
    public:
        Local(const std::string& name, bool writable = false);
        ~Local();
        unsigned int read(void* data, size_t length);
        unsigned int write(const void* data, size_t length);
        void seek(unsigned int offset);
        unsigned int tell();
        static bool exist(const std::string &name);
    };
}
}
}
#endif
