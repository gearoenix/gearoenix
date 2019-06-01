#include "phs-cns-manager.hpp"

gearoenix::physics::constraint::Manager::Manager(system::stream::Stream* const s, render::engine::Engine* const e) noexcept
    : e(e)
    , cache(s)
{
}

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::physics::constraint::Manager::get_gx3d(const core::Id, core::sync::EndCaller<Constraint>&) noexcept
{
    GXUNIMPLEMENTED
}
