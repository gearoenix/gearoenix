#include "../game-config.hpp"
#ifdef GAME_WCR
#include "../../core/cr-application.hpp"
#include <memory>

namespace gearoenix {
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace model {
        class Dynamic;
    }
}
}

class GameApp : public gearoenix::core::Application {
private:
    gearoenix::render::Engine* rndeng;
    std::shared_ptr<gearoenix::render::camera::Camera> cam;
    std::shared_ptr<gearoenix::render::model::Dynamic> mdl;
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
    GameApp(gearoenix::system::Application* sys_app);
    ~GameApp();
    void update();
    void terminate();
    void on_event(const gearoenix::core::event::Event& e);
};
#endif
