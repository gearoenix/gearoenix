#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include "cr-build-configuration.hpp"
#include "cr-types.hpp"
#ifdef IN_ANDROID
#include <android_native_app_glue.h>
#endif
namespace gearoenix {
namespace system {
    class Application;
}
namespace core {
    class Application {
    public:
        enum MouseButton {
            LEFT,
            RIGHT,
            MIDDLE,
        };
        enum ButtonAction {
            PRESS,
            RELEASE,
        };

    protected:
        system::Application* sys_app;

    public:
        Application(system::Application* sys_app);
        virtual ~Application();
        virtual void update() = 0;
        virtual void terminate() = 0;
        virtual void on_zoom(Real d);
        virtual void on_rotate(Real d);
        virtual void on_scroll(Real d);
        virtual void on_mouse(MouseButton mb, ButtonAction ba, Real x, Real y);
        virtual void on_mouse_move(Real dx, Real dy);
    };
}
}
#ifndef IN_ANDROID
#define GEAROENIX_START(CoreApp)                                                    \
    int main(int, char**)                                                           \
    {                                                                               \
        gearoenix::system::Application* app = new gearoenix::system::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        delete app;                                                                 \
        return 0;                                                                   \
    }
#elif defined(IN_WEB)
#define GEAROENIX_START(CoreApp)                                                    \
    int main(int, char**)                                                           \
    {                                                                               \
        gearoenix::system::Application* app = new gearoenix::system::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        return 0;                                                                   \
    }
#else
#define GEAROENIX_START(CoreApp)                                                         \
    void android_main(struct android_app* state)                                         \
    {                                                                                    \
        gearoenix::system::Application* app = new gearoenix::system::Application(state); \
        CoreApp* core_app = new CoreApp(app);                                            \
        app->execute(core_app);                                                          \
        delete app;                                                                      \
    }
#endif
#endif
