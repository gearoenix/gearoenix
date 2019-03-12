#include "phs-cns-manager.hpp"

gearoenix::physics::constraint::Manager::Manager(
    const std::shared_ptr<system::stream::Stream>& s,
    const std::shared_ptr<render::engine::Engine>& e)
    : e(e)
    , cache(s)
{
}

gearoenix::physics::constraint::Manager::~Manager() {}

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::physics::constraint::Manager::get_gx3d(const core::Id, core::sync::EndCaller<Constraint> &)
{
    GXUNIMPLEMENTED;
	return nullptr;
}
