#ifndef GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_HELLO_WORLD_HPP
#include <gearoenix/core/gx-cr-application.hpp>

namespace gearoenix::render::camera {
class Transformation;
}

namespace gearoenix::render::graph::tree {
class Pbr;
}

namespace gearoenix::render::scene {
class Game;
}

class GameApp final : public gearoenix::core::Application {
private:
    using GxGameScene = gearoenix::render::scene::Game;
    using GxCamTran = gearoenix::render::camera::Transformation;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;

    std::shared_ptr<GxGameScene> scn;
    GxCamTran* cam_trn = nullptr;
    std::unique_ptr<GxGrPbr> render_tree;

public:
    /// This function must be like this
    explicit GameApp(gearoenix::system::Application* sys_app) noexcept;
    ~GameApp() noexcept final = default;
    /// On every screen refresh this function is going to be called
    void update() noexcept final;
    /// Before terminating everything in engine, this function is going to be called
    void terminate() noexcept final;
};
#endif
