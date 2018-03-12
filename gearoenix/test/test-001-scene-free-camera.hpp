#include "test-config.hpp"
#ifdef TEST001
#include "../core/cr-application.hpp"

namespace gearoenix {
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace model {
        class Model;
    }
}
}

class TestApp : public gearoenix::core::Application {
private:
    gearoenix::render::Engine* rndeng;
    gearoenix::render::camera::Camera* cam;
    gearoenix::render::model::Model* mdl;
    bool left_mouse_down = false;
    class State {
    public:
        typedef enum {
            GARAGE,
            GAME,
        } Type;
    };
    State::Type state = State::GARAGE;

public:
    TestApp(gearoenix::system::Application* sys_app);
    ~TestApp();
    void update();
    void terminate();
    void on_event(const gearoenix::core::event::Event& e);
};
#endif
