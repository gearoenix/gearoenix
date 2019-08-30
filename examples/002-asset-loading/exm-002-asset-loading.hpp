#ifndef GEAROENIX_EXAMPLE_002_ASSET_LOADING_HPP
#define GEAROENIX_EXAMPLE_002_ASSET_LOADING_HPP
#include <gearoenix/core/cr-application.hpp>

namespace gearoenix::render::scene {
	class Scene;
}

class GameApp : public gearoenix::core::Application {
private:
	using GxScene = gearoenix::render::scene::Scene;

	std::shared_ptr<GxScene> scn;

public:
	/// This function must be like this
	GameApp(gearoenix::system::Application * const sys_app) noexcept;
	~GameApp() noexcept final;
	/// On every screen refresh this function is going to be called
	void update() noexcept final;
	/// Before terminationg everything in engine this function is going to be called
	void terminate() noexcept final;
};
#endif
