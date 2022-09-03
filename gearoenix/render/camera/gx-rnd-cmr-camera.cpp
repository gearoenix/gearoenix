#include "gx-rnd-cmr-camera.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include <imgui/imgui.h>

gearoenix::render::camera::Camera::Camera(
    const float target_aspect_ratio,
    const Projection projection_type,
    const float near,
    const float far) noexcept
    : core::ecs::Component(this)
    , starting_clip_ending_clip(0.0f, 0.0f, 1.0f, 1.0f)
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
    if (Projection::Perspective == projection_type) {
        const auto s = 2.0f * near * tanf(scale_fovy * 0.5f);
        projection = math::Mat4x4<float>::perspective(target_aspect_ratio * s, s, near, far);
    } else
        projection = math::Mat4x4<float>::orthographic(target_aspect_ratio * scale_fovy, scale_fovy, near, far);
    view_projection = projection * view;
}

void gearoenix::render::camera::Camera::generate_frustum_points(
    const math::Vec3<double>& l,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    std::array<math::Vec3<double>, 8>& points) const noexcept
{
    const auto scale = Projection::Perspective == projection_type ? near * tanf(scale_fovy * 0.5f) : scale_fovy * 0.5f;
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
    scale_fovy = f;
    update_projection();
}

void gearoenix::render::camera::Camera::set_scale(const float f) noexcept
{
    GX_ASSERT_D(Projection::Orthographic == projection_type);
    scale_fovy = f;
    update_projection();
}

void gearoenix::render::camera::Camera::show_gui() noexcept
{
    if (ImGui::TreeNode("Camera")) {
        bool input_changed = false;
        input_changed |= ImGui::InputFloat("far", &far, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat("near", &near, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat("aspect ratio", &target_aspect_ratio, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat(Projection::Orthographic == projection_type ? "scale" : "field of view y", &scale_fovy, 0.01f, 1.0f, "%.3f");
        if (input_changed)
            update_projection();
        ImGui::TreePop();
    }
}

void gearoenix::render::camera::Camera::set_target(std::shared_ptr<texture::Target>&& t) noexcept
{
    const auto d = t->get_dimension();
    set_target_aspect_ratio(static_cast<float>(d.x) / static_cast<float>(d.y));
    target = std::move(t);
}
