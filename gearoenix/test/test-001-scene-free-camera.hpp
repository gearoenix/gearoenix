#include "test-config.hpp"
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
    gearoenix::render::Engine* rndeng;
    gearoenix::render::camera::Camera* cam;
    bool left_mouse_down = false;
    unsigned int scene_id = 0;
    unsigned int gui_scene_id = 1;

public:
    TestApp(gearoenix::system::Application* sys_app);
    ~TestApp();
    void update();
    void terminate();
    void on_event(const gearoenix::core::event::Event& e);
};
#endif
