#include "rnd-lt-manager.hpp"

gearoenix::render::light::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
	: e(e)
    , cache(s)
{
}

gearoenix::render::light::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::render::light::Manager::get_gx3d(
    const core::Id, core::sync::EndCaller<Light> &)
{
	GXUNIMPLEMENTED;
}
