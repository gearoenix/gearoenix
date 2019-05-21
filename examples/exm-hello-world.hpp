#ifndef GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>
#include <memory>

namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace camera {
    class Perspective;
    class Transformation;
}
namespace model {
    class Model;
}
namespace scene {
    class Scene;
}
}

class GameApp : public gearoenix::core::Application {
private:
    using GxScene = gearoenix::render::scene::Scene;
    using GxCamTran = gearoenix::render::camera::Transformation;

    std::shared_ptr<GxScene> scn;
    std::shared_ptr<GxCamTran> camtrn;

public:
    /// This function must be like this
    GameApp(const std::shared_ptr<gearoenix::system::Application>& sys_app);
    ~GameApp() final;
    /// On every screen refresh this function is going to be called
    void update() final;
    /// Before terminationg everything in engine this function is going to be called
    void terminate() final;
};
#endif
