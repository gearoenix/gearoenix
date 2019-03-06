#include "../game-config.hpp"
#ifdef GX_GAME_WCR
#include "../../core/cr-application.hpp"
#include <memory>

namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
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
    const std::shared_ptr<gearoenix::render::engine::Engine> rndeng;
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
    GameApp(const std::shared_ptr<gearoenix::system::Application> &sys_app);
    ~GameApp();
    void update();
    void terminate();
    void on_event(const gearoenix::core::event::Event& e);
};
#endif
