#include "rnd-cmr-transformation.hpp"
#include "../../math/math-projector-frustum.hpp"
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
    on_projection_update = f;
}

void gearoenix::render::camera::Transformation::look_at(const math::Vec3& target, const math::Vec3& up) noexcept
{
    look_at(uniform->position, target, up);
}

void gearoenix::render::camera::Transformation::look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up) noexcept
{
    uniform->position = origin;
    uniform->z = (origin - target).normalized();
    uniform->x = up.cross(uniform->z).normalized();
    uniform->y = uniform->z.cross(uniform->x);
    uniform->view = math::Mat4x4::look_at(uniform->position, target, uniform->y);
    uniform->inversed_rotation = uniform->view * math::Mat4x4::translator(uniform->position);
    update_view_projection();
}

const gearoenix::math::Vec3& gearoenix::render::camera::Transformation::get_x_axis() const noexcept
{
    return uniform->x;
}

const gearoenix::math::Vec3& gearoenix::render::camera::Transformation::get_y_axis() const noexcept
{
    return uniform->y;
}

const gearoenix::math::Vec3& gearoenix::render::camera::Transformation::get_z_axis() const noexcept
{
    return uniform->z;
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
    uniform->position += uniform->x * t;
    update_location();
}

void gearoenix::render::camera::Transformation::local_y_translate(const core::Real t) noexcept
{
    uniform->position += uniform->y * t;
    update_location();
}

void gearoenix::render::camera::Transformation::local_z_translate(const core::Real t) noexcept
{
    uniform->position += uniform->z * t;
    update_location();
}

void gearoenix::render::camera::Transformation::global_rotate(const core::Real rad, const math::Vec3& axis, const math::Vec3& location) noexcept
{
    math::Mat4x4 rot = math::Mat4x4::rotation(axis, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(axis, rad);
    uniform->x = irot * uniform->x;
    uniform->y = irot * uniform->y;
    uniform->z = irot * uniform->z;
    uniform->position = (irot * (uniform->position - location)) + location;
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::global_rotate(const core::Real rad, const math::Vec3& axis) noexcept
{
    global_rotate(rad, axis, uniform->position);
}

void gearoenix::render::camera::Transformation::local_rotate(const core::Real rad, const math::Vec3& vec) noexcept
{
    math::Mat4x4 rot = math::Mat4x4::rotation(vec, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(vec, rad);
    uniform->x = irot * uniform->x;
    uniform->y = irot * uniform->y;
    uniform->z = irot * uniform->z;
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_x_rotate(const core::Real rad) noexcept
{
    math::Mat4x4 rot = math::Mat4x4::rotation(uniform->x, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(uniform->x, rad);
    uniform->y = irot * uniform->y;
    uniform->z = irot * uniform->z;
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_y_rotate(const core::Real rad) noexcept
{
    math::Mat4x4 rot = math::Mat4x4::rotation(uniform->y, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(uniform->y, rad);
    uniform->x = irot * uniform->x;
    uniform->z = irot * uniform->z;
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::local_z_rotate(const core::Real rad) noexcept
{
    math::Mat4x4 rot = math::Mat4x4::rotation(uniform->z, -rad);
    math::Mat4x4 irot = math::Mat4x4::rotation(uniform->z, rad);
    uniform->x = irot * uniform->x;
    uniform->y = irot * uniform->y;
    uniform->inversed_rotation *= rot;
    update_location();
}

void gearoenix::render::camera::Transformation::set_orientation(const math::Quat& q) noexcept
{
    const math::Mat4x4 r = q.to_mat();
    uniform->x = r * uniform->x;
    uniform->y = r * uniform->y;
    uniform->z = r * uniform->z;
    uniform->inversed_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    update_location();
}
