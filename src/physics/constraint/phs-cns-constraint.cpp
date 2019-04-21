#include "phs-cns-constraint.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "phs-cns-placer.hpp"

gearoenix::physics::constraint::Constraint::Constraint(const core::Id my_id, const Type::Id t)
    : core::asset::Asset(my_id, core::asset::Type::CONSTRAINT)
    , t(t)
{
}

gearoenix::physics::constraint::Constraint::~Constraint()
{
}

const std::vector<std::shared_ptr<gearoenix::physics::body::Body>> gearoenix::physics::constraint::Constraint::get_all_bodies() const
{
    return std::vector<std::shared_ptr<body::Body>>();
}

bool gearoenix::physics::constraint::Constraint::is_alive() const
{
    return alive;
}

void gearoenix::physics::constraint::Constraint::apply(const core::Real)
{
    applied = true;
}

gearoenix::physics::constraint::Constraint* gearoenix::physics::constraint::Constraint::read(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<render::engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    const Type::Id t = f->read<Type::Id>();
    switch (t) {
    case Type::PLACER:
        GXUNEXPECTED;
        //return new Placer(t, my_id, f, e, c);
    default:
        GXUNEXPECTED;
    }
}
