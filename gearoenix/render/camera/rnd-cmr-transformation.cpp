#include "rnd-cmr-transformation.hpp"
#include "../../physics/collider/phs-cld-frustum.hpp"
#include "rnd-cmr-uniform.hpp"
#include <utility>

gearoenix::render::camera::Transformation::Transformation(
    Uniform* const uniform,
    physics::collider::Frustum* const frustum_collider,
    std::vector<std::array<math::Vec3, 4>>* const cascade) noexcept
    : uniform(uniform)
    , frustum_collider(frustum_collider)
    , cascaded_shadow_frustum_partitions(cascade)
{
}

void gearoenix::render::camera::Transformation::update_location() noexcept
{
    math::Mat4x4 translate = math::Mat4x4::translator(-(uniform->position));
    uniform->view = uniform->inversed_rotation * translate;
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
    frustum_collider->set_view_projection(uniform->view_projection);
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

void gearoenix::render::camera::Transformation::look_at(const math::Vec3& target, const math::Vec3& up) noexcept
{
    look_at(uniform->position, target, up);
}

void gearoenix::render::camera::Transformation::look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up) noexcept
{
    uniform->position = origin;
    z_axis = (origin - target).normalized();
    x_axis = up.cross(z_axis).normalized();
    y_axis = z_axis.cross(x_axis);
    uniform->view = math::Mat4x4::look_at(uniform->position, target, y_axis);
    uniform->inversed_rotation = uniform->view * math::Mat4x4::translator(uniform->position);
    update_view_projection();
}

const gearoenix::math::Vec3& gearoenix::render::camera::Transformation::get_location() const noexcept
{
    return uniform->position;
}

void gearoenix::render::camera::Transformation::get_location(math::Vec3& l) const noexcept
{
    l = uniform->position;
}

void gearoenix::render::camera::Transformation::set_location(const math::Vec3& l) noexcept
{
    uniform->position = l;
    update_location();
}

void gearoenix::render::camera::Transformation::translate(const math::Vec3& vec) noexcept
{
    uniform->position += vec;
    update_location();
}

void gearoenix::render::camera::Transformation::local_x_translate(const core::Real t) noexcept
{
    uniform->position += x_axis * t;
    update_location();
}

void gearoenix::render::camera::Transformation::local_y_translate(const core::Real t) noexcept
{
    uniform->position += y_axis * t;
    update_location();
}

void gearoenix::render::camera::Transformation::local_z_translate(const core::Real t) noexcept
{
    uniform->position += z_axis * t;
    update_location();
}

void gearoenix::render::camera::Transformation::global_rotate(const core::Real rad, const math::Vec3& axis, const math::Vec3& location) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4::rotation(axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4::rotation(axis, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0f)).xyz();
    y_axis = (irot * math::Vec4(y_axis, 0.0f)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0f)).xyz();
    uniform->position = (irot * math::Vec4(uniform->position - location, 1.0f)).xyz() + location;
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_rotate(const core::Real rad, const math::Vec3& vec) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4::rotation(vec, -rad);
    const math::Mat4x4 irot = math::Mat4x4::rotation(vec, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0f)).xyz();
    y_axis = (irot * math::Vec4(y_axis, 0.0f)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0f)).xyz();
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_x_rotate(const core::Real rad) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4::rotation(x_axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4::rotation(x_axis, rad);
    y_axis = (irot * math::Vec4(y_axis, 0.0f)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0f)).xyz();
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_y_rotate(const core::Real rad) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4::rotation(y_axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4::rotation(y_axis, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0f)).xyz();
    z_axis = (irot * math::Vec4(z_axis, 0.0f)).xyz();
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_z_rotate(const core::Real rad) noexcept
{
    const math::Mat4x4 rot = math::Mat4x4::rotation(z_axis, -rad);
    const math::Mat4x4 irot = math::Mat4x4::rotation(z_axis, rad);
    x_axis = (irot * math::Vec4(x_axis, 0.0f)).xyz();
    y_axis = (irot * math::Vec4(y_axis, 0.0f)).xyz();
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::set_orientation(const math::Quat& q) noexcept
{
    const math::Mat4x4 r = q.to_mat();
    x_axis = (r * math::Vec4::X).xyz();
    y_axis = (r * math::Vec4::Y).xyz();
    z_axis = (r * math::Vec4::Z).xyz();
    uniform->inversed_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    update_location();
}
