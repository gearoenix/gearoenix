#include "phs-bd-body.hpp"

gearoenix::physics::body::Body::Body(const Behaviour::Type body_type)
    : body_type(body_type)
{
}

gearoenix::physics::body::Body::~Body()
{
}

gearoenix::physics::body::Body::Behaviour::Type gearoenix::physics::body::Body::get_type() const
{
    return body_type;
}
