#ifndef GEAROENIX_EXAMPLE_002_ASSET_LOADING_HPP
#define GEAROENIX_EXAMPLE_002_ASSET_LOADING_HPP
#include <gearoenix/core/gx-cr-application.hpp>

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::render::camera {
class JetController;
}

namespace gearoenix::render::graph::tree {
class Pbr;
}

class GameApp final : public gearoenix::core::Application {
private:
    using GxScene = gearoenix::render::scene::Scene;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;
    using GxCamCtrl = gearoenix::render::camera::JetController;

    std::shared_ptr<GxScene> scn;
    std::unique_ptr<GxGrPbr> tree;
    std::shared_ptr<GxCamCtrl> cam_ctrl;

public:
    /// This function must be like this
    GameApp(gearoenix::system::Application* sys_app) noexcept;
    ~GameApp() noexcept final;
    /// On every screen refresh this function is going to be called
    void update() noexcept final;
    /// Before terminationg everything in engine this function is going to be called
    void terminate() noexcept final;
};
#endif
