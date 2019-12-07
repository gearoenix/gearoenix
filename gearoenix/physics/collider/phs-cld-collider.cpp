#include "phs-cld-collider.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../phs-transformation.hpp"
#include "phs-cld-mesh.hpp"

void gearoenix::physics::collider::Collider::update_box() noexcept
{
    origin_box.get_all_corners(updated_points);
    updated_box.reset();
    for (math::Vec3& p : updated_points) {
        updated_box.put_without_update((model_matrix * math::Vec4(p, 1.0f)).xyz());
    }
    updated_box.update();
}

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(system::stream::Stream* const f) noexcept
{
    switch (f->read<Type>()) {
    case Type::Ghost:
        return nullptr;
    case Type::Mesh:
        return new Mesh(f);
    default:
        GXUNEXPECTED
    }
}

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Collider::hit(const math::Ray3& r, const core::Real d_min) const noexcept
{
    return updated_box.hit(r, d_min);
}

bool gearoenix::physics::collider::Collider::check_intersection(const math::Aabb3& box) const noexcept
{
    return updated_box.check_intersection(box);
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Collider::check_intersection_status(const math::Aabb3& box) const noexcept
{
    return updated_box.check_intersection_status(box);
}

void gearoenix::physics::collider::Collider::put_in_box(const math::Aabb3& b) noexcept
{
    origin_box.put(b);
    update_box();
}

const gearoenix::math::Vec3& gearoenix::physics::collider::Collider::get_location() const noexcept
{
    return updated_box.get_center();
}

void gearoenix::physics::collider::Collider::set_location(const math::Vec3& l) noexcept
{
    model_matrix.set_location(l);
    updated_box.set_center(l);
}

void gearoenix::physics::collider::Collider::local_scale(const core::Real s) noexcept
{
    scale *= s;
    updated_box.set_diameter(updated_box.get_diameter() * s);
    model_matrix.local_scale(s);
}

void gearoenix::physics::collider::Collider::local_x_scale(const core::Real s) noexcept
{
    scale[0] *= s;
    math::Vec3 d = updated_box.get_diameter();
    d[0] *= s;
    updated_box.set_diameter(d);
    model_matrix.local_x_scale(s);
}

void gearoenix::physics::collider::Collider::set_model_matrix(const math::Mat4x4& m) noexcept
{
    scale = ((m * math::Vec4(1.0f, 1.0f, 1.0f, 0.0f)).xyz()).abs();
    model_matrix = m;
    origin_box.get_all_corners(updated_points);
    updated_box.reset();
    for (math::Vec3& p : updated_points) {
        p = (model_matrix * math::Vec4(p, 1.0f)).xyz();
        updated_box.put(p);
    }
}
