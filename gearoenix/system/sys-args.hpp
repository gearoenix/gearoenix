#ifndef GEAROENIX_SYSTEM_ARGS_HPP
#define GEAROENIX_SYSTEM_ARGS_HPP
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-static.hpp"
#include <string>
#include <vector>

namespace gearoenix::system {
class Args {
    GX_GET_CREF_PRV(std::string, process_name)
    GX_GET_CREF_PRV(std::vector<std::string>, tokens)
public:
    Args(const int argc, const char* const* const argv) noexcept
        : process_name(argv[0])
        , tokens(argc - 1)
    {
        for (int ai = 1, ti = 0; ai < argc; ++ai, ++ti)
            tokens[ti] = std::string(argv[ai]);
    }
};
}

#endif