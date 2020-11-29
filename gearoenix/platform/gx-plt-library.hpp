#ifndef GEAROENIX_PLATFORM_LIBRARY_HPP
#define GEAROENIX_PLATFORM_LIBRARY_HPP

#include "gx-plt-build-configuration.hpp"

#ifndef GX_PLT_WEB
#define GX_SHARED_LINKAGE_SUPPORTED

namespace gearoenix::platform {
struct Library {
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
FunctionPtr gearoenix::platform::Library::load(const char* function_name) noexcept
{
    return reinterpret_cast<FunctionPtr>(raw_load(function_name));
}

#endif
#endif