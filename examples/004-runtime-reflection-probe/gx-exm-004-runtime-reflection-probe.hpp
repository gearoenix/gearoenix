#ifndef GEAROENIX_EXAMPLE_004_RUNTIME_REFLECTION_PROBE_HPP
#define GEAROENIX_EXAMPLE_004_RUNTIME_REFLECTION_PROBE_HPP
#include <gearoenix/core/gx-cr-application.hpp>

namespace gearoenix::render::camera {
class Camera;
class JetController;
}

namespace gearoenix::render::graph::tree {
class Pbr;
}

namespace gearoenix::render::scene {
class Game;
}

class Example004RuntimeReflectionProbeApp final : public gearoenix::core::Application {
private:
    using GxGameScene = gearoenix::render::scene::Game;
    using GxCam = gearoenix::render::camera::Camera;
    using GxCamCtrl = gearoenix::render::camera::JetController;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;

    std::shared_ptr<GxGameScene> scn;
    std::shared_ptr<GxCamCtrl> cam_ctrl;
    std::unique_ptr<GxGrPbr> render_tree;

public:
    explicit Example004RuntimeReflectionProbeApp(gearoenix::system::Application* sys_app) noexcept;
    ~Example004RuntimeReflectionProbeApp() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
};
#endif
