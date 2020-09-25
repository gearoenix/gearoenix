#include "gx-sys-library.hpp"
#ifdef GX_IN_WINDOWS
#include <Windows.h>
#elif defined(GX_IN_LINUX) || defined(GX_IN_ANDROID)
#include <dlfcn.h>
#endif

gearoenix::system::Library::Library() noexcept = default;

gearoenix::system::Library* gearoenix::system::Library::construct(const char* name) noexcept
{
#ifdef GX_IN_WINDOWS
    HMODULE lib = LoadLibrary(name);
#else
    void* const lib = dlopen(name, RTLD_NOW | RTLD_LOCAL);
#endif
    if (lib == nullptr)
        return nullptr;
    auto l = new Library();
    l->lib = lib;
    return l;
}

gearoenix::system::Library::~Library() noexcept
{
#ifdef GX_IN_WINDOWS
    FreeLibrary(reinterpret_cast<HMODULE>(lib));
#else
    dlclose(lib);
#endif
}

void* gearoenix::system::Library::raw_load(const char* function_name) noexcept
{
#ifdef GX_IN_WINDOWS
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(lib), function_name));
#else
    return dlsym(lib, function_name)
#endif
}
