#include "phs-cns-placer.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::constraint::Placer::Placer(system::File* f, core::EndCaller<core::EndCallerIgnore> c)
    : Constraint(PLACER)
{
    f->read(t);
    switch (t) {
    case DOWN_MIDDLE:
        parameters = new core::Real[2];
        f->read(parameters[0]);
        f->read(parameters[1]);
    default:
        UNEXPECTED;
    }
    TODO;
}

gearoenix::physics::constraint::Placer::~Placer()
{
    if (nullptr != parameters)
        delete[] parameters;
    parameters = nullptr;
}

void gearoenix::physics::constraint::Placer::on_event(const core::event::Event*)
{
    TODO;
}
