#pragma once
#include "../platform/gx-plt-application.hpp"
#include "gx-cr-singleton.hpp"

#ifdef GX_PLATFORM_WEBASSEMBLY
#define GEAROENIX_START(ApplicationMainClass) GX_MAIN_ENTRY(                 \
    auto plt_app = new gearoenix::platform::Application(GX_MAIN_ENTRY_ARGS); \
    plt_app->run(new ApplicationMainClass(*plt_app)))
#else
#define GEAROENIX_START(ApplicationMainClass) GX_MAIN_ENTRY(                               \
    auto plt_app = std::make_unique<gearoenix::platform::Application>(GX_MAIN_ENTRY_ARGS); \
    plt_app->run(new ApplicationMainClass()))
#endif

namespace gearoenix::core {
struct Application : Singleton<Application> {
    Application();
    ~Application() override;
    virtual void update();
};
}
