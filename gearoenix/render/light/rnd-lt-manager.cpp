#include "rnd-lt-manager.hpp"

gearoenix::render::light::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
	: e(e)
    , cache(s)
{
}

gearoenix::render::light::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::render::light::Manager::get(
	const core::Id mid, const core::sync::EndCaller<Light> &c) 
{
	GXUNIMPLEMENTED;
}
