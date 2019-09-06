#ifndef GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#define GEAROENIX_EXAMPLE_003_NET_HELLO_WORLD_HPP
#include <gearoenix/core/cr-application.hpp>
#include <gearoenix/core/event/cr-ev-listner.hpp>

namespace gearoenix::render::camera {
class Camera;
class Transformation;
}

namespace gearoenix::render::scene {
class Scene;
}

class GameApp : 
	public gearoenix::core::Application, 
	public gearoenix::core::event::Listner {
private:
    using GxScene = gearoenix::render::scene::Scene;
	using GxCam = gearoenix::render::camera::Camera;
	using GxCamTran = gearoenix::render::camera::Transformation;

    std::shared_ptr<GxScene> scn;
    std::shared_ptr<GxCamTran> camtrn;
	std::shared_ptr<GxCam> cam;

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
