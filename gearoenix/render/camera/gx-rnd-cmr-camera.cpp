#include "gx-rnd-cmr-camera.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../texture/gx-rnd-txt-target.hpp"

gearoenix::render::camera::Camera::Camera(
    const float target_aspect_ratio,
    const Projection projection_type,
    const float near,
    const float far) noexcept
    : core::ecs::Component(this)
    , target_aspect_ratio(target_aspect_ratio)
    , far(far)
    , near(near)
    , projection_type(projection_type)
{
    update_projection();
}

gearoenix::render::camera::Camera::Camera(Camera&&) noexcept = default;

gearoenix::render::camera::Camera::~Camera() noexcept = default;

void gearoenix::render::camera::Camera::set_view(const math::Mat4x4<float>& v) noexcept
{
    view = v;
    view_projection = projection * v;
}

void gearoenix::render::camera::Camera::set_target_aspect_ratio(const float tap) noexcept
{
    target_aspect_ratio = tap;
    update_projection();
}

void gearoenix::render::camera::Camera::set_projection_type(const Projection p) noexcept
{
    projection_type = p;
    update_projection();
}

void gearoenix::render::camera::Camera::update_projection() noexcept
{
    if (Projection::Perspective == projection_type)
        projection = math::Mat4x4<float>::perspective(target_aspect_ratio * scale, scale, near, far);
    else
        projection = math::Mat4x4<float>::orthographic(target_aspect_ratio, scale, near, far);
    view_projection = projection * view;
}

void gearoenix::render::camera::Camera::generate_frustum_points(
    const math::Vec3<double>& l,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    std::array<math::Vec3<double>, 8>& points) const noexcept
{
    const auto fpn = Projection::Perspective == projection_type ? (static_cast<double>(far) + static_cast<double>(near)) * static_cast<double>(scale) / static_cast<double>(near) : static_cast<double>(scale);
    const auto far_x = static_cast<double>(target_aspect_ratio) * fpn;
    const auto far_y = fpn;
    const auto nz = z * -static_cast<double>(near);
    const auto fz = z * -static_cast<double>(far);
    const auto nx = x * (static_cast<double>(target_aspect_ratio) * static_cast<double>(scale));
    const auto fx = x * far_x;
    const auto ny = y * static_cast<double>(scale);
    const auto fy = y * far_y;
    points[0] = l - fx + fy + fz;
    points[1] = l + fx + fy + fz;
    points[2] = l - fx - fy + fz;
    points[3] = l + fx - fy + fz;
    points[4] = l - nx + ny + nz;
    points[5] = l + nx + ny + nz;
    points[6] = l - nx - ny + nz;
    points[7] = l + nx - ny + nz;
}

void gearoenix::render::camera::Camera::set_near(const float f) noexcept
{
    near = f;
    update_projection();
}

void gearoenix::render::camera::Camera::set_far(const float f) noexcept
{
    far = f;
    update_projection();
}

void gearoenix::render::camera::Camera::set_yfov(const float f) noexcept
{
    GX_ASSERT_D(Projection::Perspective == projection_type);
    scale = 2.0f * near * std::tanf(f);
    update_projection();
}

void gearoenix::render::camera::Camera::set_target(std::shared_ptr<texture::Target>&& t) noexcept
{
    target = std::move(t);
}
