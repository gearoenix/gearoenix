#include "phs-bd-body.hpp"

#include <utility>

gearoenix::physics::body::Body::Body(const core::Id my_id, std::string name, const Behaviour body_type, std::shared_ptr<Transferable> body_obj)
    : core::asset::Asset(my_id, core::asset::Type::Body, std::move(name))
    , body_type(body_type)
    , body_obj(std::move(body_obj))
{
}

gearoenix::physics::body::Body::~Body() = default;

gearoenix::physics::body::Body::Behaviour gearoenix::physics::body::Body::get_body_type() const
{
    return body_type;
}

const std::shared_ptr<gearoenix::physics::Transferable>& gearoenix::physics::body::Body::get_body_obj() const
{
    return body_obj;
}
