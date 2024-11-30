#pragma once
#include "gx-plt-build-configuration.hpp"
#ifndef GX_PLATFORM_WEBASSEMBLY
#define GX_SHARED_LINKAGE_SUPPORTED

namespace gearoenix::platform {
struct Library final {
private:
    void* lib = nullptr;
    Library();

public:
    static Library* construct(const char* name);
    ~Library();
    void* raw_load(const char* function_name);

    template <typename FunctionPtr>
    FunctionPtr load(const char* function_name);
};
}

template <typename FunctionPtr>
FunctionPtr gearoenix::platform::Library::load(const char* function_name)
{
    return reinterpret_cast<FunctionPtr>(raw_load(function_name));
}
#endif