#include "gx-phs-cld-aabb.hpp"

gearoenix::physics::collider::Aabb3::Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower) noexcept
    : core::ecs::Component(this)
    , original_box(upper, lower)
    , updated_box(original_box)
{
}

gearoenix::physics::collider::Aabb3::Aabb3(const math::Aabb3<double>& original_box) noexcept
    : core::ecs::Component(this)
    , original_box(original_box)
    , updated_box(original_box)
{
}

gearoenix::physics::collider::Aabb3::Aabb3(Aabb3&&) noexcept = default;

gearoenix::physics::collider::Aabb3::~Aabb3() noexcept = default;