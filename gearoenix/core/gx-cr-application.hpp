#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include "gx-cr-build-configuration.hpp"
#include "gx-cr-types.hpp"
#include <memory>
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include <android_native_app_glue.h>
#elif defined(GX_USE_WINAPI)
#include <Windows.h>
#elif defined(GX_USE_SDL)
#include <SDL_main.h>
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
    platform::Application* platform_application = nullptr;
    render::engine::Engine* render_engine = nullptr;

public:
    Application(platform::Application* sys_app) noexcept;
    virtual ~Application() noexcept;
    virtual void update() noexcept;
    virtual void terminate() noexcept;
};
}

/*
#ifdef GX_IN_WEB
#define GEAROENIX_START(CoreApp)                                                    \
    int main(int, char**)                                                           \
    {                                                                               \
        gearoenix::platform::Application* app = new gearoenix::platform::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        return 0;                                                                   \
    }
*/
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#define GEAROENIX_START(CoreApp)                                                    \
    void android_main(struct android_app* state)                                    \
    {                                                                               \
        const auto app = std::make_unique<gearoenix::platform::Application>(state); \
        app->execute(std::make_unique<CoreApp>(app.get()));                         \
    }
#elif defined(GX_USE_WINAPI)
#define GEAROENIX_START(CoreApp)                                                                                     \
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)                                                             \
    {                                                                                                                \
        const std::shared_ptr<gearoenix::platform::Application> app = gearoenix::platform::Application::construct(); \
        const std::shared_ptr<CoreApp> core_app(new CoreApp(app));                                                   \
        app->execute(core_app);                                                                                      \
        return 0;                                                                                                    \
    }
#elif defined(GX_IN_LINUX) || defined(GX_IN_MAC) || defined(GX_IN_IOS) || defined(GX_USE_SDL)
#define GEAROENIX_START(CoreApp)                                                         \
    extern "C" int main(int argc, char* argv[])                                          \
    {                                                                                    \
        const auto app = std::make_unique<gearoenix::platform::Application>(argc, argv); \
        app->execute(std::make_unique<CoreApp>(app.get()));                              \
        return 0;                                                                        \
    }
#else
#error "Unexpected platform."
#endif
#endif
