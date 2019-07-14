#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include "cr-build-configuration.hpp"
#include "cr-types.hpp"
#include <memory>
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#include <android_native_app_glue.h>
#elif defined(GX_USE_WINAPI)
#include <Windows.h>
#elif defined(GX_USE_SDL)
#include <SDL.h>
#endif
namespace gearoenix {
namespace render {
    namespace engine {
        class Engine;
    }
}
namespace system {
    class Application;
}
namespace core {
    namespace event {
        class Event;
    }
    class Application {
    public:
    protected:
        std::shared_ptr<system::Application> sys_app;
        std::shared_ptr<render::engine::Engine> rnd_eng;

    public:
        Application(const std::shared_ptr<system::Application>& sys_app);
        virtual ~Application();
        virtual void update() = 0;
        virtual void terminate();
    };
}
}
// #ifdef GX_IN_WEB
// #define GEAROENIX_START(CoreApp)                                                    \
//     int main(int, char**)                                                           \
//     {                                                                               \
//         gearoenix::system::Application* app = new gearoenix::system::Application(); \
//         CoreApp* core_app = new CoreApp(app);                                       \
//         app->execute(core_app);                                                     \
//         return 0;                                                                   \
//     }
#if defined(GX_IN_ANDROID) && !defined(GX_USE_SDL)
#define GEAROENIX_START(CoreApp)                                                         \
    void android_main(struct android_app* state)                                         \
    {                                                                                    \
        gearoenix::system::Application* app = new gearoenix::system::Application(state); \
        CoreApp* core_app = new CoreApp(app);                                            \
        app->execute(core_app);                                                          \
        delete app;                                                                      \
    }
#elif defined(GX_USE_WINAPI)
#define GEAROENIX_START(CoreApp)                                                                                 \
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)                                                         \
    {                                                                                                            \
        const std::shared_ptr<gearoenix::system::Application> app = gearoenix::system::Application::construct(); \
        const std::shared_ptr<CoreApp> core_app(new CoreApp(app));                                               \
        app->execute(core_app);                                                                                  \
        return 0;                                                                                                \
    }
#elif defined(GX_IN_LINUX) || defined(GX_IN_MAC) || defined(GX_IN_IOS) || defined(GX_USE_SDL)
#define GEAROENIX_START(CoreApp)                                                                                 \
    int main(int, char**)                                                                                        \
    {                                                                                                            \
        const std::shared_ptr<gearoenix::system::Application> app = gearoenix::system::Application::construct(); \
        const std::shared_ptr<CoreApp> core_app(new CoreApp(app));                                               \
        app->execute(core_app);                                                                                  \
        return 0;                                                                                                \
    }
#else
#error "Unexpected platform."
#endif
#endif
