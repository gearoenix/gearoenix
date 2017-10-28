#include "config.hpp"
#ifdef TEST001
#include "../core/cr-application.hpp"

namespace gearoenix {
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
}
}

class TestApp : public gearoenix::core::Application {
private:
    gearoenix::render::Engine* eng;
    gearoenix::render::camera::Camera* cam;
    unsigned int scene_id = 0;
    bool left_mouse_down = false;

public:
    TestApp(gearoenix::system::Application* sys_app);
    ~TestApp();
    void update();
    void terminate();
    void on_zoom(gearoenix::core::Real d);
    void on_rotate(gearoenix::core::Real d);
    void on_scroll(gearoenix::core::Real d);
    void on_mouse(MouseButton mb, ButtonAction ba, gearoenix::core::Real x, gearoenix::core::Real y);
    void on_mouse_move(gearoenix::core::Real dx, gearoenix::core::Real dy);
};
#endif
