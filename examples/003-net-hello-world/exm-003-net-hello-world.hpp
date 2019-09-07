#ifndef GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>
#include <gearoenix/core/event/cr-ev-listner.hpp>

namespace gearoenix::render::camera {
class Camera;
class Transformation;
}

namespace gearoenix::render::scene {
class Game;
class Ui;
}

namespace gearoenix::render::widget {
class Modal;
}

class GameApp : 
	public gearoenix::core::Application, 
	public gearoenix::core::event::Listner {
private:
    using GxGameScene = gearoenix::render::scene::Game;
    using GxUiScene = gearoenix::render::scene::Ui;
	using GxCam = gearoenix::render::camera::Camera;
    using GxCamTran = gearoenix::render::camera::Transformation;
    using GxModal = gearoenix::render::widget::Modal;

    std::shared_ptr<GxGameScene> scn;
    std::shared_ptr<GxUiScene> uiscn;
    std::shared_ptr<GxCamTran> camtrn;
	std::shared_ptr<GxCam> cam;
    std::shared_ptr<GxModal> modal;

public:
    /// This function must be like this
    GameApp(gearoenix::system::Application* sys_app) noexcept;
    ~GameApp() noexcept final = default;
    /// On every screen refresh this function is going to be called
    void update() noexcept final;
    /// Before terminationg everything in engine this function is going to be called
    void terminate() noexcept final;
	/// Listner function
	bool on_event(const gearoenix::core::event::Data& event_data) noexcept;
};
#endif
