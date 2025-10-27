#pragma once
#include "../platform/gx-plt-application.hpp"
#include "../platform/gx-plt-main-entry.hpp"
#include "gx-cr-singleton.hpp"

#ifdef GX_PLATFORM_WEBASSEMBLY
#define GEAROENIX_START(ApplicationMainClass)                  \
    void gearoenix::platform::main_entry()                     \
    {                                                          \
        auto plt_app = new gearoenix::platform::Application(); \
        plt_app->run(new ApplicationMainClass()));             \
    }                                                          \
    static_assert(true, "")
#else
#define GEAROENIX_START(ApplicationMainClass)                                \
    void gearoenix::platform::main_entry()                                   \
    {                                                                        \
        auto plt_app = std::make_unique<gearoenix::platform::Application>(); \
        plt_app->run(new ApplicationMainClass());                            \
    }                                                                        \
    static_assert(true, "")
#endif

namespace gearoenix::core {
struct Application : Singleton<Application> {
    Application();
    ~Application() override;
    virtual void update();
};
}
