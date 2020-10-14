#ifndef GEAROENIX_SYSTEM_LIBRARY_HPP
#define GEAROENIX_SYSTEM_LIBRARY_HPP

#include "../core/gx-cr-build-configuration.hpp"

#ifndef GX_IN_WEB
#define GX_SHARED_LINKAGE_SUPPORTED

namespace gearoenix::system {
class Library {
private:
    void* lib = nullptr;
    Library() noexcept;

public:
    static Library* construct(const char* name) noexcept;
    ~Library() noexcept;
    void* raw_load(const char* function_name) noexcept;

    template <typename FunctionPtr>
    FunctionPtr load(const char* function_name) noexcept;
};
}

template <typename FunctionPtr>
FunctionPtr gearoenix::system::Library::load(const char* function_name) noexcept
{
    return reinterpret_cast<FunctionPtr>(raw_load(function_name));
}

#endif
#endif