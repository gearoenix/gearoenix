#ifndef GEAROENIX_TOOL_IBL_BAKER_HPP
#define GEAROENIX_TOOL_IBL_BAKER_HPP
#include <atomic>
#include <gearoenix/core/cr-application.hpp>
#include <gearoenix/core/cr-types.hpp>
#include <gearoenix/core/event/cr-ev-listener.hpp>
#include <gearoenix/math/math-vertex.hpp>
#include <random>

namespace gearoenix::render::camera {
class Camera;
class Transformation;
}

namespace gearoenix::render::graph::tree {
class Pbr;
}

namespace gearoenix::render::scene {
class Game;
class Ui;
}

namespace gearoenix::render::model {
class Static;
}

namespace gearoenix::render::skybox {
class Equirectangular;
}

namespace gearoenix::render::widget {
class Button;
class Modal;
class Text;
class Edit;
}

class Example004RuntimeReflectionProbeApp final : public gearoenix::core::Application,
                                                  public gearoenix::core::event::Listener {
private:
    using GxGameScene = gearoenix::render::scene::Game;
    using GxStaticModel = gearoenix::render::model::Static;
    using GxUiScene = gearoenix::render::scene::Ui;
    using GxCam = gearoenix::render::camera::Camera;
    using GxCamTran = gearoenix::render::camera::Transformation;
    using GxModal = gearoenix::render::widget::Modal;
    using GxButton = gearoenix::render::widget::Button;
    using GxTextWdg = gearoenix::render::widget::Text;
    using GxEditWdg = gearoenix::render::widget::Edit;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;
    using GxVec2 = gearoenix::math::Vec2<double>;
    using GxVec3 = gearoenix::math::Vec3<double>;
    using GxVec4 = gearoenix::math::Vec4<double>;
    using GxSkyEqrect = gearoenix::render::skybox::Equirectangular;

    std::shared_ptr<GxGameScene> scn;
    std::shared_ptr<GxGameScene> obj_scn;
    std::shared_ptr<GxUiScene> uiscn;
    GxCamTran* cam_trn = nullptr;
    std::shared_ptr<GxCam> cam;
    GxCamTran* obj_cam_trn = nullptr;
    std::shared_ptr<GxCam> obj_cam;
    std::shared_ptr<GxEditWdg> file_location;
    std::shared_ptr<GxSkyEqrect> sky;
    std::unique_ptr<GxGrPbr> render_tree;
    bool camera_rotation_enabled = false;
    std::chrono::high_resolution_clock::time_point last_time_item_detail_modal_closed = std::chrono::high_resolution_clock::now();
    double camera_forward = 0.0;
    double camera_sideward = 0.0;

    void on_open() noexcept;

public:
    explicit Example004RuntimeReflectionProbeApp(gearoenix::system::Application* sys_app) noexcept;
    ~Example004RuntimeReflectionProbeApp() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    bool on_event(const gearoenix::core::event::Data& event_data) noexcept final;
};
#endif
