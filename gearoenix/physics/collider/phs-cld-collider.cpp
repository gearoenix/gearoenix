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
        p = model_matrix * p;
        updated_box.put_without_update(p);
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

void gearoenix::physics::collider::Collider::set_location(const math::Vec3& l) noexcept
{
    model_matrix.set_location(l);
    updated_box.set_center(l);
}

void gearoenix::physics::collider::Collider::local_scale(const core::Real s) noexcept
{
    updated_box.set_diameter(updated_box.get_diameter() * s);
    model_matrix.local_scale(s);
}

void gearoenix::physics::collider::Collider::local_x_scale(const core::Real s) noexcept
{
    updated_box.set_diameter(updated_box.get_diameter() * math::Vec3(s, 1.0f, 1.0f));
    model_matrix.local_x_scale(s);
}