#ifndef GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>
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
        class Model;
    }
    namespace scene {
        class Scene;
    }
}
}

class GameApp : public gearoenix::core::Application {
private:
    std::shared_ptr<gearoenix::render::scene::Scene> scn;
    std::shared_ptr<gearoenix::render::camera::Camera> cam;
    std::shared_ptr<gearoenix::render::model::Model> mdl;

public:
	/// This function must be like this
    GameApp(const std::shared_ptr<gearoenix::system::Application>& sys_app);
    ~GameApp() override final;
	/// On every screen refresh this function is going to be called
    void update() override final;
	/// Before terminationg everything in engine this function is going to be called
    void terminate() override final;
};
#endif
