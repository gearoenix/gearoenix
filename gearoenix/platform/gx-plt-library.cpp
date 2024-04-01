#include "gx-plt-library.hpp"
#ifdef GX_SHARED_LINKAGE_SUPPORTED
#ifdef GX_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

gearoenix::platform::Library::Library() = default;

gearoenix::platform::Library* gearoenix::platform::Library::construct(const char* name)
{
#ifdef GX_PLATFORM_WINDOWS
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

gearoenix::platform::Library::~Library()
{
#ifdef GX_PLATFORM_WINDOWS
    FreeLibrary(reinterpret_cast<HMODULE>(lib));
#else
    dlclose(lib);
#endif
}

void* gearoenix::platform::Library::raw_load(const char* function_name)
{
#ifdef GX_PLATFORM_WINDOWS
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(lib), function_name));
#else
    return dlsym(lib, function_name);
#endif
}
#endif