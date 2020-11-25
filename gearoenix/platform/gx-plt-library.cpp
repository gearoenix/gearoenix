#include "gx-plt-library.hpp"
#ifdef GX_SHARED_LINKAGE_SUPPORTED
#ifdef GX_PLT_WINAPI
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

gearoenix::platform::Library::Library() noexcept = default;

gearoenix::platform::Library* gearoenix::platform::Library::construct(const char* name) noexcept
{
#ifdef GX_PLT_WINAPI
    auto* const lib = static_cast<void*>(LoadLibraryA(name));
#else
    void* const lib = dlopen(name, RTLD_NOW | RTLD_LOCAL);
#endif
    if (lib == nullptr)
        return nullptr;
    auto l = new Library();
    l->lib = lib;
    return l;
}

gearoenix::platform::Library::~Library() noexcept
{
#ifdef GX_PLT_WINAPI
    FreeLibrary(reinterpret_cast<HMODULE>(lib));
#else
    dlclose(lib);
#endif
}

void* gearoenix::platform::Library::raw_load(const char* function_name) noexcept
{
#ifdef GX_PLT_WINAPI
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(lib), function_name));
#else
    return dlsym(lib, function_name);
#endif
}
#endif