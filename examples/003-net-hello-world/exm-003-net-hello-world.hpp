#ifndef GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>
#include <gearoenix/core/cr-types.hpp>
#include <gearoenix/core/event/cr-ev-listner.hpp>
#include <gearoenix/math/math-vertex.hpp>
#include <atomic>
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

namespace gearoenix::render::widget {
class Button;
class Modal;
class Text;
}

class GameApp : public gearoenix::core::Application,
                public gearoenix::core::event::Listner {
private:
    using GxGameScene = gearoenix::render::scene::Game;
    using GxUiScene = gearoenix::render::scene::Ui;
    using GxCam = gearoenix::render::camera::Camera;
    using GxCamTran = gearoenix::render::camera::Transformation;
    using GxModal = gearoenix::render::widget::Modal;
    using GxButton = gearoenix::render::widget::Button;
    using GxTextWdg = gearoenix::render::widget::Text;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;
	using GxReal = gearoenix::core::Real;
    using GxVec2 = gearoenix::math::Vec2;
    using GxVec3 = gearoenix::math::Vec3;
    using GxVec4 = gearoenix::math::Vec4;


	struct ShelfInfo {
		std::uniform_real_distribution<GxReal> rand_genx;
		std::uniform_real_distribution<GxReal> rand_genz;
		GxReal y = 0.0f;
	};

    std::shared_ptr<GxGameScene> scn;
    std::shared_ptr<GxUiScene> uiscn;
    std::shared_ptr<GxCamTran> camtrn;
    std::shared_ptr<GxCam> cam;
    std::shared_ptr<GxModal> modal;
    std::shared_ptr<GxButton> look_at_button;
    std::shared_ptr<GxTextWdg> text_location;
    std::unique_ptr<GxGrPbr> render_tree;
    std::atomic<bool> showing_object_details = false;
	std::atomic<GxReal> camera_forward = 0.0f;
	std::atomic<GxReal> camera_sideward = 0.0f;

    void translate_camera(const GxVec3& t);

public:
    /// This function must be like this
    GameApp(gearoenix::system::Application* sys_app) noexcept;
    ~GameApp() noexcept final;
    /// On every screen refresh this function is going to be called
    void update() noexcept final;
    /// Before terminationg everything in engine this function is going to be called
    void terminate() noexcept final;
    /// Listner function
    bool on_event(const gearoenix::core::event::Data& event_data) noexcept;
};
#endif
