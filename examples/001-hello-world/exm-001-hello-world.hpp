#ifndef GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>

namespace gearoenix::render::camera {
class Transformation;
}

namespace gearoenix::render::graph::tree {
class Pbr;
}

namespace gearoenix::render::scene {
class Game;
}

class GameApp : public gearoenix::core::Application {
private:
    using GxGameScene = gearoenix::render::scene::Game;
    using GxCamTran = gearoenix::render::camera::Transformation;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;

    std::shared_ptr<GxGameScene> scn;
    GxCamTran* camtrn = nullptr;
    std::unique_ptr<GxGrPbr> render_tree;

public:
    /// This function must be like this
    GameApp(gearoenix::system::Application* sys_app) noexcept;
    ~GameApp() noexcept final = default;
    /// On every screen refresh this function is going to be called
    void update() noexcept final;
    /// Before terminationg everything in engine this function is going to be called
    void terminate() noexcept final;
};
#endif
