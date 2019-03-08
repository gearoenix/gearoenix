#include "phs-cns-manager.hpp"

gearoenix::physics::constraint::Manager::Manager(
    const std::shared_ptr<system::stream::Stream>& s,
    const std::shared_ptr<render::engine::Engine>& e)
    : e(e)
    , s(s)
{
    GXUNIMPLEMENTED;
}

gearoenix::physics::constraint::Manager::~Manager() {}

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::physics::constraint::Manager::get(const core::Id mid, const core::sync::EndCaller<Constraint> c)
{
    GXUNIMPLEMENTED;
}