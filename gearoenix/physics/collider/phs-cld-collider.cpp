#include "phs-cld-collider.hpp"
#include "phs-cld-mesh.hpp"

void gearoenix::physics::collider::Collider::update_box() noexcept
{
    origin_box.get_all_corners(updated_points);
    updated_box.reset((model_matrix * math::Vec4(updated_points[0], 1.0)).xyz());
    for (std::size_t i = 1; i < GX_COUNT_OF(updated_points); ++i) {
        updated_box.put_without_update((model_matrix * math::Vec4(updated_points[i], 1.0)).xyz());
    }
    updated_box.update();
}

gearoenix::physics::collider::Collider::Collider(const Type t) noexcept
    : collider_type(t)
{
}

gearoenix::physics::collider::Collider::Collider(const Type t, const Collider& c) noexcept
    : collider_type(t)
    , origin_box(c.origin_box)
    , updated_box(c.updated_box)
    , model_matrix(c.model_matrix)
    , parent(c.parent)
{
}

gearoenix::physics::collider::Collider::Collider(const Collider& o) noexcept
    : collider_type(o.collider_type)
    , origin_box(o.origin_box)
    , updated_box(o.updated_box)
    , model_matrix(o.model_matrix)
    , parent(o.parent)
    , updated_points {
        o.updated_points[0],
        o.updated_points[1],
        o.updated_points[2],
        o.updated_points[3],
        o.updated_points[4],
        o.updated_points[5],
        o.updated_points[6],
        o.updated_points[7],
    }
{}

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(system::stream::Stream* const f) noexcept
{
    switch (f->read<Type>()) {
    case Type::Ghost:
        return nullptr;
    case Type::Mesh:
        return new Mesh(f);
    default:
        GX_UNEXPECTED
    }
}

std::optional<double> gearoenix::physics::collider::Collider::hit(const math::Ray3& r, const double d_min) const noexcept
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

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::clone() const noexcept
{
    GX_UNIMPLEMENTED // It's never going to be implemented, the inherited class must do it.
}

void gearoenix::physics::collider::Collider::put_in_box(const math::Aabb3& b) noexcept
{
    origin_box.put(b);
    update_box();
}

gearoenix::math::Vec3<double> gearoenix::physics::collider::Collider::get_location() const noexcept
{
    return model_matrix.get_location();
}

void gearoenix::physics::collider::Collider::set_location(const math::Vec3<double>& l) noexcept
{
    model_matrix.set_location(l);
    updated_box.set_center(l);
}

void gearoenix::physics::collider::Collider::local_z_rotate(const double d) noexcept
{
    model_matrix *= math::Mat4x4<double>::rotation(z_axis, d);
    update_box();
}

void gearoenix::physics::collider::Collider::local_scale(const double s) noexcept
{
    current_local_scale *= s;
    updated_box.set_diameter(updated_box.get_diameter() * s);
    model_matrix.local_scale(s);
    on_scale();
}

void gearoenix::physics::collider::Collider::local_scale(const math::Vec3<double>& s) noexcept
{
    current_local_scale *= s;
    updated_box.set_diameter(updated_box.get_diameter() * s);
    model_matrix.local_scale(s);
    on_scale();
}

void gearoenix::physics::collider::Collider::local_x_scale(const double s) noexcept
{
    current_local_scale.x *= s;
    auto d = updated_box.get_diameter();
    d.x *= s;
    updated_box.set_diameter(d);
    model_matrix.local_x_scale(s);
    on_scale();
}

void gearoenix::physics::collider::Collider::local_y_scale(const double s) noexcept
{
    current_local_scale.y *= s;
    auto d = updated_box.get_diameter();
    d.y *= s;
    updated_box.set_diameter(d);
    model_matrix.local_y_scale(s);
    on_scale();
}

void gearoenix::physics::collider::Collider::local_z_scale(const double s) noexcept
{
    current_local_scale.z *= s;
    auto d = updated_box.get_diameter();
    d.z *= s;
    updated_box.set_diameter(d);
    model_matrix.local_z_scale(s);
    on_scale();
}

void gearoenix::physics::collider::Collider::set_model_matrix(const math::Mat4x4<double>& m) noexcept
{
    current_local_scale = ((m * math::Vec4(1.0, 1.0, 1.0, 0.0)).xyz()).abs();
    model_matrix = m;
    origin_box.get_all_corners(updated_points);
    updated_box.reset();
    for (auto& p : updated_points) {
        p = (model_matrix * math::Vec4(p, 1.0)).xyz();
        updated_box.put_without_update(p);
    }
    updated_box.update();
    on_scale();
}
