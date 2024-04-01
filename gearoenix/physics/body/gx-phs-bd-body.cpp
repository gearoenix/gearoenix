#include "gx-phs-bd-body.hpp"
#include "../../render/model/gx-rnd-mdl-dynamic.hpp"
#include <utility>

gearoenix::physics::body::Body::Body(
    const core::Id id,
    std::string name,
    const Type body_type,
    std::shared_ptr<render::model::Dynamic> model)
    : core::asset::Asset(id, core::asset::Type::Body, std::move(name))
    , body_type(body_type)
    , model(std::move(model))
{
}

gearoenix::physics::body::Body::~Body() = default;