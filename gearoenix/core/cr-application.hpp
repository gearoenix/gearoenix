#ifndef GEAROENIX_CORE_APPLICATION_HPP
#define GEAROENIX_CORE_APPLICATION_HPP
#include "cr-types.hpp"
namespace gearoenix {
namespace system {
    class Application;
}
namespace core {
    class Application {
    protected:
        system::Application* sys_app;

    public:
        Application(system::Application* sys_app);
        virtual ~Application();
        virtual void update() = 0;
        virtual void terminate() = 0;
        virtual void on_zoom(Real d);
        virtual void on_rotate(Real d);
    };
}
}

#define GEAROENIX_START(CoreApp)                                                    \
    int main(int, char**)                                                           \
    {                                                                               \
        gearoenix::system::Application* app = new gearoenix::system::Application(); \
        CoreApp* core_app = new CoreApp(app);                                       \
        app->execute(core_app);                                                     \
        delete app;                                                                 \
        return 0;                                                                   \
    }
#endif
