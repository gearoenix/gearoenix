#include "gx-rnd-cmr-camera.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../gx-rnd-vertex.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include <boost/mp11/algorithm.hpp>
#include <imgui/imgui.h>
#include <random>

static thread_local std::random_device rd;
static thread_local std::uniform_real_distribution<float> urd(0.5f, 1.0f);
static thread_local std::default_random_engine re(rd());

gearoenix::render::camera::Camera::Camera(
    engine::Engine& e,
    const std::string& name,
    const std::size_t resolution_cfg_listener,
    const core::sync::EndCaller& end_caller,
    std::shared_ptr<texture::Target>&& customised_target,
    const Projection projection_type,
    const float near,
    const float far) noexcept
    : core::ecs::Component(this)
    , e(e)
    , starting_clip_ending_clip(0.0f, 0.0f, 1.0f, 1.0f)
    , has_customised_target(nullptr == customised_target)
    , target(nullptr == customised_target ? e.get_texture_manager()->create_default_camera_render_target(name, end_caller) : std::move(customised_target))
    , target_aspect_ratio(target->get_aspect_ratio())
    , far(far)
    , near(near)
    , colour_tuning(HdrTuneMappingGammaCorrection {})
    , projection_type(projection_type)
    , debug_colour {
        urd(re),
        urd(re),
        urd(re),
        urd(re),
    }
    , resolution_cfg_listener(resolution_cfg_listener)
{
    update_projection();
}

gearoenix::render::camera::Camera::Camera(Camera&& o) noexcept
    : core::ecs::Component(this)
    , e(o.e)
    , view(o.view)
    , projection(o.projection)
    , view_projection(o.view_projection)
    , starting_clip_ending_clip(o.starting_clip_ending_clip)
    , has_customised_target(o.has_customised_target)
    , target(std::move(o.target))
    , has_customised_target_aspect_ratio(o.has_customised_target_aspect_ratio)
    , target_aspect_ratio(o.target_aspect_ratio)
    , reference_id(o.reference_id)
    , scene_id(o.scene_id)
    , flag(o.flag)
    , far(o.far)
    , near(o.near)
    , scale_fov_y(o.scale_fov_y)
    , colour_tuning(o.colour_tuning)
    , projection_type(o.projection_type)
    , layer(o.layer)
    , usage(o.usage)
    , debug_enabled(o.debug_enabled)
    , debug_colour(o.debug_colour)
    , debug_mesh(o.debug_mesh)
    , has_bloom(o.has_bloom)
    , resolution_cfg_listener(o.resolution_cfg_listener)
{
    o.resolution_cfg_listener = 0;
}

gearoenix::render::camera::Camera::~Camera() noexcept
{
    e.get_platform_application().get_base().get_configuration().get_render_configuration().get_runtime_resolution().remove_listener(resolution_cfg_listener);
}

void gearoenix::render::camera::Camera::generate_frustum_points(
    const math::Vec3<double>& l,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    std::array<math::Vec3<double>, 8>& points) const noexcept
{
    const auto scale = Projection::Perspective == projection_type ? near * tanf(scale_fov_y * 0.5f) : scale_fov_y * 0.5f;
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

void gearoenix::render::camera::Camera::set_view(const math::Mat4x4<float>& v) noexcept
{
    view = v;
    view_projection = projection * v;
}

void gearoenix::render::camera::Camera::set_customised_target_aspect_ratio(const float tar) noexcept
{
    has_customised_target_aspect_ratio = true;
    set_target_aspect_ratio(tar);
}

void gearoenix::render::camera::Camera::set_projection_type(const Projection p) noexcept
{
    projection_type = p;
    update_projection();
}

void gearoenix::render::camera::Camera::update_projection() noexcept
{
    if (Projection::Perspective == projection_type) {
        const auto s = 2.0f * near * tanf(scale_fov_y * 0.5f);
        projection = math::Mat4x4<float>::perspective(target_aspect_ratio * s, s, near, far);
    } else
        projection = math::Mat4x4<float>::orthographic(target_aspect_ratio * scale_fov_y, scale_fov_y, near, far);
    view_projection = projection * view;

    if (debug_enabled) {
        create_debug_mesh();
    }
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

void gearoenix::render::camera::Camera::set_fov_y(const float f) noexcept
{
    GX_ASSERT_D(Projection::Perspective == projection_type);
    scale_fov_y = f;
    update_projection();
}

void gearoenix::render::camera::Camera::set_scale(const float f) noexcept
{
    GX_ASSERT_D(Projection::Orthographic == projection_type);
    scale_fov_y = f;
    update_projection();
}

void gearoenix::render::camera::Camera::show_gui() noexcept
{
    if (ImGui::TreeNode("Camera")) {
        bool input_changed = false;
        input_changed |= ImGui::InputFloat("Far", &far, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat("Near", &near, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat("Aspect Ratio", &target_aspect_ratio, 0.01f, 1.0f, "%.3f");
        /// TODO
        // input_changed |= ImGui::InputFloat("HDR Tune Mapping", &hdr_tune_mapping, 0.01f, 1.0f, "%.6f");
        // input_changed |= ImGui::InputFloat("Gamma Correction", &gamma_correction, 0.01f, 1.0f, "%.6f");
        input_changed |= ImGui::InputFloat(Projection::Orthographic == projection_type ? "Scale" : "Field Of View Y", &scale_fov_y, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::Checkbox("Show debug mesh", &debug_enabled);
        if (input_changed)
            update_projection();
        ImGui::TreePop();
    }
}

void gearoenix::render::camera::Camera::enable_debug_mesh() noexcept
{
    if (!debug_enabled) {
        debug_enabled = true;
        create_debug_mesh();
    }
}

void gearoenix::render::camera::Camera::disable_debug_mesh() noexcept
{
    debug_enabled = false;
}

void gearoenix::render::camera::Camera::set_has_customised_target_aspect_ratio(const bool b) noexcept
{
    has_customised_target_aspect_ratio = b;
    update_target_aspect_ratio();
}

gearoenix::math::Ray3<double> gearoenix::render::camera::Camera::generate_ray(
    const physics::Transformation& transform, const math::Vec2<double>& normalised_point) const noexcept
{
    const auto scale = static_cast<double>(projection_type == Projection::Perspective ? (2.0f * near * tanf(scale_fov_y * 0.5f)) : scale_fov_y);
    const auto near_plane_point = normalised_point * scale;
    const auto direction = (transform.get_x_axis() * near_plane_point.x) + (transform.get_y_axis() * near_plane_point.y) + (transform.get_z_axis() * static_cast<double>(-near));
    const auto origin = transform.get_local_location() + direction;
    return { origin, projection_type == Projection::Perspective ? direction.normalised() : -transform.get_z_axis() };
}

void gearoenix::render::camera::Camera::update_target_aspect_ratio() noexcept
{
    if (!has_customised_target_aspect_ratio) {
        auto td = target->get_aspect_ratio();
        const auto clip = starting_clip_ending_clip.zw() - starting_clip_ending_clip.xy();
        set_target_aspect_ratio(td * clip.x / clip.y);
    }
}

void gearoenix::render::camera::Camera::set_target_aspect_ratio(const float tap) noexcept
{
    target_aspect_ratio = tap;
    update_projection();
}

void gearoenix::render::camera::Camera::set_customised_target(std::shared_ptr<texture::Target>&& t) noexcept
{
    has_customised_target = true;
    target = std::move(t);
    update_target_aspect_ratio();
}

void gearoenix::render::camera::Camera::update_target() noexcept
{
    if (has_customised_target)
        return;
    GX_TODO; // we need to be able to store name in the component and for that we need a virtual move function
    target = e.get_texture_manager()->create_default_camera_render_target("camera", core::sync::EndCaller([] {}));
    update_target_aspect_ratio();
}

void gearoenix::render::camera::Camera::create_debug_mesh() noexcept
{
    std::array<math::Vec3<double>, 8> points;
    generate_frustum_points(
        math::Vec3<double>(0.0),
        math::X3D<double>,
        math::Y3D<double>,
        math::Z3D<double>,
        points);
    std::vector<PbrVertex> vertices(points.size());
    for (std::size_t i = 0; i < points.size(); ++i)
        vertices[i].position = math::Vec3<float>(points[i]);
    std::vector<std::uint32_t> indices {
        0, 1, 0, 2, 1, 3, 2, 3, // far
        0, 4, 1, 5, 2, 6, 3, 7, // edges
        4, 5, 4, 6, 5, 7, 6, 7, // near
    };
    std::string mesh_name = "camera-debug-mesh" + std::to_string(reinterpret_cast<std::size_t>(this));
    (void)e.get_mesh_manager()->remove_if_exist(mesh_name);
    const std::shared_ptr<decltype(debug_mesh)> result_holder(new decltype(debug_mesh)());
    *result_holder = e.get_mesh_manager()->build(
        std::move(mesh_name),
        std::move(vertices),
        std::move(indices),
        core::sync::EndCaller([this, result_holder] {
            if (nullptr != *result_holder)
                debug_mesh = *result_holder;
        }));
    if (nullptr != *result_holder)
        debug_mesh = *result_holder;
}
