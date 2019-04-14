#include "cr-application.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../system/sys-app.hpp"

gearoenix::core::Application::Application(const std::shared_ptr<system::Application>& sys_app)
    : sys_app(sys_app)
    , rnd_eng(sys_app->get_render_engine())
{
}

gearoenix::core::Application::~Application() {}

void gearoenix::core::Application::terminate()
{
	sys_app = nullptr;
	rnd_eng = nullptr;
}
