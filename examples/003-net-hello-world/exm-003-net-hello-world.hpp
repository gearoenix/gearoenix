#ifndef GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>

namespace gearoenix::render::camera {
class Transformation;
}

namespace gearoenix::render::scene {
class Scene;
}

class GameApp : public gearoenix::core::Application {
private:
    using GxScene = gearoenix::render::scene::Scene;
    using GxCamTran = gearoenix::render::camera::Transformation;

    std::shared_ptr<GxScene> scn;
    std::shared_ptr<GxCamTran> camtrn;

public:
    /// This function must be like this
    GameApp(const std::shared_ptr<gearoenix::system::Application>& sys_app) noexcept;
    ~GameApp() noexcept final = default;
    /// On every screen refresh this function is going to be called
    void update() noexcept final;
    /// Before terminationg everything in engine this function is going to be called
    void terminate() noexcept final;
};
#endif
