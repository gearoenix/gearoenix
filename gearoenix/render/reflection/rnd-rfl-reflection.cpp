#include "rnd-rfl-reflection.hpp"
#include "../../physics/collider/phs-cld-sphere.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"

gearoenix::render::reflection::Reflection::Reflection(
    const core::Id id,
    const Type t,
    render::engine::Engine* const e,
    const math::Vec3<double>& position,
    const double influence_radius) noexcept
    : Asset(id, core::asset::Type::Reflection)
    , reflection_type(t)
    , e(e)
    , collider(new physics::collider::Sphere(position, influence_radius))
{
}

gearoenix::render::reflection::Reflection::~Reflection() noexcept = default;
