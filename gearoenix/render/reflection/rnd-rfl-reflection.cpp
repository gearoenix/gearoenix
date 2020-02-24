#include "rnd-rfl-reflection.hpp"
#include "../../physics/collider/phs-cld-sphere.hpp"

gearoenix::render::reflection::Reflection::Reflection(
    const core::Id id,
    const Type t,
    render::engine::Engine* const e,
    const math::Vec3& position,
    core::Real influence_radius) noexcept
    : Asset(id, core::asset::Type::Reflection)
    , reflection_type(t)
    , e(e)
    , collider(new physics::collider::Sphere(position, influence_radius))
{
}

gearoenix::render::reflection::Reflection::~Reflection() noexcept = default;
