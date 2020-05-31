#include "phs-cns-manager.hpp"

gearoenix::physics::constraint::Manager::Manager(std::unique_ptr<system::stream::Stream> s, render::engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

gearoenix::physics::constraint::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::physics::constraint::Manager::get_gx3d(const core::Id, core::sync::EndCaller<Constraint>&) noexcept
{
    (void)this;
    GX_UNIMPLEMENTED
}
