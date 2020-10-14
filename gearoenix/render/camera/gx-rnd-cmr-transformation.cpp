#include "gx-rnd-cmr-transformation.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "gx-rnd-cmr-uniform.hpp"
#include <utility>

gearoenix::render::camera::Transformation::Transformation(
    Uniform* const uniform,
    physics::collider::Frustum* const frustum_collider,
    std::vector<std::array<math::Vec3<double>, 4>>* const cascade) noexcept
    : uniform(uniform)
    , frustum_collider(frustum_collider)
    , cascaded_shadow_frustum_partitions(cascade)
{
}

void gearoenix::render::camera::Transformation::update_location() noexcept
{
    const auto translate = math::Mat4x4<float>::translator(-(uniform->position));
    uniform->view = uniform->inverted_rotation * translate;
    update_view_projection();
}

void gearoenix::render::camera::Transformation::update_projection() noexcept
{
    on_projection_update();
    update_view_projection();
}

void gearoenix::render::camera::Transformation::update_view_projection() noexcept
{
    uniform->view_projection = uniform->projection * uniform->view;
    uniform->uniform_view_projection = math::Mat4x4(
                                           0.5f, 0.0f, 0.0f, 0.0f,
                                           0.0f, 0.5f, 0.0f, 0.0f,
                                           0.0f, 0.0f, 1.0f, 0.0f,
                                           0.5f, 0.5f, 0.0f, 1.0f)
        * uniform->view_projection;
    update_frustum();
}

void gearoenix::render::camera::Transformation::update_frustum() noexcept
{
    on_frustum_update();
}

void gearoenix::render::camera::Transformation::set_on_frustum_update(std::function<void()> f) noexcept
{
    on_frustum_update = std::move(f);
}

void gearoenix::render::camera::Transformation::set_on_projection_update(std::function<void()> f) noexcept
{
    on_projection_update = std::move(f);
}

void gearoenix::render::camera::Transformation::look_at(const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept
{
    look_at(math::Vec3<double>(uniform->position), target, up);
}

void gearoenix::render::camera::Transformation::look_at(const math::Vec3<double>& origin, const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept
{
    uniform->position = math::Vec3<float>(origin);
    z_axis = (origin - target).normalized();
    x_axis = up.cross(z_axis).normalized();
    y_axis = z_axis.cross(x_axis);
    uniform->view = math::Mat4x4<float>::look_at(uniform->position, math::Vec3<float>(target), math::Vec3<float>(y_axis));
    uniform->inverted_rotation = uniform->view * math::Mat4x4<float>::translator(uniform->position);
    update_view_projection();
}

gearoenix::math::Vec3<double> gearoenix::render::camera::Transformation::get_location() const noexcept
{
    GX_UNEXPECTED // It can not provide this functionality
}

void gearoenix::render::camera::Transformation::get_location(math::Vec3<double>& l) const noexcept
{
    l = math::Vec3<double>(uniform->position);
}

void gearoenix::render::camera::Transformation::set_location(const math::Vec3<double>& l) noexcept
{
    uniform->position = math::Vec3<float>(l);
    update_location();
}

void gearoenix::render::camera::Transformation::translate(const math::Vec3<double>& vec) noexcept
{
    uniform->position += math::Vec3<float>(vec);
    update_location();
}

void gearoenix::render::camera::Transformation::local_x_translate(const double t) noexcept
{
    uniform->position += math::Vec3<float>(x_axis * t);
    update_location();
}

void gearoenix::render::camera::Transformation::local_y_translate(const double t) noexcept
{
    uniform->position += math::Vec3<float>(y_axis * t);
    update_location();
}

void gearoenix::render::camera::Transformation::local_z_translate(const double t) noexcept
{
    uniform->position += math::Vec3<float>(z_axis * t);
    update_location();
}

void gearoenix::render::camera::Transformation::global_rotate(const double rad, const math::Vec3<double>& axis, const math::Vec3<double>& location) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4<double>::rotation(axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4<double>::rotation(axis, rad);
    x_axis = (irot * math::Vec4<double>(x_axis, 0.0)).xyz();
    y_axis = (irot * math::Vec4<double>(y_axis, 0.0)).xyz();
    z_axis = (irot * math::Vec4<double>(z_axis, 0.0)).xyz();
    uniform->position = math::Vec3<float>((irot * math::Vec4(math::Vec3<double>(uniform->position) - location, 1.0)).xyz() + location);
    uniform->inverted_rotation *= math::Mat4x4<float>(rot);
    update_location();
}

void gearoenix::render::camera::Transformation::local_rotate(const double rad, const math::Vec3<double>& vec) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4<double>::rotation(vec, -rad);
    const math::Mat4x4 irot = math::Mat4x4<double>::rotation(vec, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (irot * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0)).xyz();
    uniform->inverted_rotation *= math::Mat4x4<float>(rot);
    update_location();
}

void gearoenix::render::camera::Transformation::local_x_rotate(const double rad) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4<double>::rotation(x_axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4<double>::rotation(x_axis, rad);
    y_axis = (irot * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0)).xyz();
    uniform->inverted_rotation *= math::Mat4x4<float>(rot);
    update_location();
}

void gearoenix::render::camera::Transformation::local_y_rotate(const double rad) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4<double>::rotation(y_axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4<double>::rotation(y_axis, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0)).xyz();
    uniform->inverted_rotation *= math::Mat4x4<float>(rot);
    update_location();
}

void gearoenix::render::camera::Transformation::local_z_rotate(const double rad) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4<double>::rotation(z_axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4<double>::rotation(z_axis, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (irot * math::Vec4(y_axis, 0.0)).xyz();
    uniform->inverted_rotation *= math::Mat4x4<float>(rot);
    update_location();
}

void gearoenix::render::camera::Transformation::set_orientation(const math::Quat<double>& q) noexcept
{
    const math::Mat4x4 r = q.to_mat();
    x_axis = (r * math::Vec4(1.0, 0.0, 0.0, 0.0)).xyz();
    y_axis = (r * math::Vec4(0.0, 1.0, 0.0, 0.0)).xyz();
    z_axis = (r * math::Vec4(0.0, 0.0, 1.0, 0.0)).xyz();
    uniform->inverted_rotation = math::Mat4x4<float>(math::Quat(q.x, q.y, q.z, -q.w).to_mat());
    update_location();
}
