#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include "cr-build-configuration.hpp"
#include "cr-types.hpp"
#ifdef IN_ANDROID
#include <android_native_app_glue.h>
#elif defined(IN_WINDOWS)
#include <Windows.h>
#endif
namespace gearoenix {
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
        system::Application* sys_app;

    public:
        Application(system::Application* sys_app);
        virtual ~Application();
        virtual void update() = 0;
        virtual void terminate() = 0;
        virtual void on_event(const event::Event& e) = 0;
    };
}
}
#ifdef IN_WEB
#define GEAROENIX_START(CoreApp)                                                    \
    int main(int, char**)                                                           \
    {                                                                               \
        gearoenix::system::Application* app = new gearoenix::system::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        return 0;                                                                   \
    }
#elif defined(IN_ANDROID)
#define GEAROENIX_START(CoreApp)                                                         \
    void android_main(struct android_app* state)                                         \
    {                                                                                    \
        gearoenix::system::Application* app = new gearoenix::system::Application(state); \
        CoreApp* core_app = new CoreApp(app);                                            \
        app->execute(core_app);                                                          \
        delete app;                                                                      \
    }
#elif defined(IN_WINDOWS)
#define GEAROENIX_START(CoreApp)                                                    \
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)                            \
    {                                                                               \
        gearoenix::system::Application* app = new gearoenix::system::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        delete app;                                                                 \
        return 0;                                                                   \
    }
#elif defined(IN_LINUX) || defined(IN_MAC) || defined(IN_IOS)
#define GEAROENIX_START(CoreApp)                                                    \
    int main(int, char**)                                                           \
    {                                                                               \
        gearoenix::system::Application* app = new gearoenix::system::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        delete app;                                                                 \
        return 0;                                                                   \
    }
#else
#error "Unexpected platform."
#endif
#endif
