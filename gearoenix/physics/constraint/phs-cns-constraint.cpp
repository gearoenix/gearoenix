#include "phs-cns-constraint.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "phs-cns-placer.hpp"

gearoenix::physics::constraint::Constraint::Constraint(core::Id my_id, Type t)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::CONSTRAINT)
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

void gearoenix::physics::constraint::Constraint::apply(core::Real)
{
    applied = true;
}

gearoenix::physics::constraint::Constraint* gearoenix::physics::constraint::Constraint::read(core::Id my_id, system::stream::Stream* f, render::Engine* render_engine, core::EndCaller<core::EndCallerIgnore> c)
{
    Type t;
    f->read(t);
    switch (t) {
    case PLACER:
        return new Placer(my_id, f, render_engine, c);
    default:
        UNEXPECTED;
    }
}

gearoenix::physics::constraint::Placer* gearoenix::physics::constraint::Constraint::to_placer()
{
    if (t == PLACER)
        return reinterpret_cast<Placer*>(this);
    return nullptr;
}

const gearoenix::physics::constraint::Placer* gearoenix::physics::constraint::Constraint::to_placer() const
{
    if (t == PLACER)
        return reinterpret_cast<const Placer*>(this);
    return nullptr;
}
