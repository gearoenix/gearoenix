#include "gx-phs-cns-manager.hpp"

gearoenix::physics::constraint::Manager::Manager(std::unique_ptr<platform::stream::Stream> s, render::engine::Engine* const e)
    : e(e)
    , cache(std::move(s))
{
}

gearoenix::physics::constraint::Manager::~Manager() = default;

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::physics::constraint::Manager::get_gx3d(const core::Id, core::job::EndCaller<Constraint>&)
{
    (void)this;
    GX_UNIMPLEMENTED
}
