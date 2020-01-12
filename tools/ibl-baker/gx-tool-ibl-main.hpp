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

namespace gearoenix::render::widget {
class Button;
class Modal;
class Text;
class Edit;
}

class IblBakerApp : public gearoenix::core::Application,
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
    using GxReal = gearoenix::core::Real;
    using GxVec2 = gearoenix::math::Vec2;
    using GxVec3 = gearoenix::math::Vec3;
    using GxVec4 = gearoenix::math::Vec4;

    std::shared_ptr<GxGameScene> scn;
    std::shared_ptr<GxUiScene> uiscn;
    GxCamTran* camtrn = nullptr;
    std::shared_ptr<GxCam> cam;
    std::shared_ptr<GxEditWdg> file_location;
    std::unique_ptr<GxGrPbr> render_tree;
    std::chrono::high_resolution_clock::time_point last_time_item_detail_modal_closed = std::chrono::high_resolution_clock::now();
    GxReal camera_forward = 0.0f;
    GxReal camera_sideward = 0.0f;

    void on_open() noexcept;

public:
    explicit IblBakerApp(gearoenix::system::Application* sys_app) noexcept;
    ~IblBakerApp() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    bool on_event(const gearoenix::core::event::Data& event_data) noexcept final;
};
#endif
