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

void gearoenix::physics::collider::Aabb3::update(const math::Mat4x4<double>& transform) noexcept
{
    std::array<math::Vec3<double>, 8> corners;
    original_box.get_all_corners(corners);
    auto tmp = transform * math::Vec4(corners[0], 1.0);
    updated_box.reset(tmp.xyz());
    for (std::size_t i = 0; i < 8; ++i) {
        tmp = transform * math::Vec4(corners[0], 1.0);
        updated_box.put_without_update(tmp.xyz());
    }
    updated_box.update();
}