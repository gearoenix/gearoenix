#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include "../platform/gx-plt-application.hpp"
#include "gx-cr-build-configuration.hpp"
#include "gx-cr-types.hpp"
#include <memory>

#ifdef GX_PLATFORM_WEBASSEMBLY
#define GEAROENIX_START(ApplicationMainClass) GX_MAIN_ENTRY(                 \
    auto plt_app = new gearoenix::platform::Application(GX_MAIN_ENTRY_ARGS); \
    plt_app->run(new ApplicationMainClass(*plt_app)))
#else
#define GEAROENIX_START(ApplicationMainClass) GX_MAIN_ENTRY(                               \
    auto plt_app = std::make_unique<gearoenix::platform::Application>(GX_MAIN_ENTRY_ARGS); \
    plt_app->run(new ApplicationMainClass(*plt_app.get())))
#endif

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::core::event {
struct Event;
}

namespace gearoenix::core {
struct Application {
public:
protected:
    platform::Application& platform_application;
    render::engine::Engine& render_engine;

public:
    explicit Application(platform::Application& platform_application) noexcept;
    virtual ~Application() noexcept;
    virtual void update() noexcept;
};
}
#endif
