#include "gx-rnd-cmr-camera.hpp"
#include "../../core/gx-cr-string.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../gx-rnd-vertex.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include <boost/mp11/algorithm.hpp>
#include <imgui/imgui.h>
#include <random>

namespace {
thread_local std::random_device rd;
thread_local std::uniform_real_distribution urd(0.5f, 1.0f);
thread_local std::default_random_engine re(rd());
}

gearoenix::render::camera::Camera::Camera(
    engine::Engine& e,
    const std::type_index final_type,
    const std::string& name,
    Target&& target,
    const core::ecs::entity_id_t entity_id)
    : Component(final_type, std::string(name), entity_id)
    , e(e)
    , starting_clip_ending_clip(0.0f, 0.0f, 1.0f, 1.0f)
    , target(std::move(target))
    , colour_tuning(GammaCorrection {})
    , debug_colour {
        urd(re),
        urd(re),
        urd(re),
        urd(re),
    }
{
    if (this->target.is_customised()) {
        exposure.disable();
    }
}

gearoenix::render::camera::Camera::~Camera()
{
    e.get_platform_application().get_base().get_configuration().get_render_configuration().get_runtime_resolution().remove_observer(resolution_cfg_observer);
}

void gearoenix::render::camera::Camera::set_component_self(const std::shared_ptr<Component>& c)
{
    camera_self = std::dynamic_pointer_cast<Camera>(c);
    if (0 == resolution_cfg_observer) {
        resolution_cfg_observer = e.get_platform_application().get_base().get_configuration().get_render_configuration().get_runtime_resolution().add_observer(
            [c = camera_self](const Resolution&) {
                auto cam = c.lock();
                if (nullptr == cam) {
                    return false;
                }
                cam->update_target(core::job::EndCaller([] { }));
                return true;
            });
    }
    Component::set_component_self(c);
}

void gearoenix::render::camera::Camera::generate_frustum_points(
    const math::Vec3<double>& location,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    std::array<math::Vec3<double>, 8>& points) const
{
    const auto [scale, fpn] = [&] {
        if (const auto* const p = std::get_if<PerspectiveProjectionData>(&projection_data)) {
            const auto s = static_cast<double>(near * tanf(p->field_of_view_y * 0.5f));
            return std::make_pair(s, (static_cast<double>(far) + static_cast<double>(near)) * s / static_cast<double>(near));
        }
        if (const auto* const o = std::get_if<OrthographicProjectionData>(&projection_data)) {
            const auto s = static_cast<double>(o->scale * 0.5f);
            return std::make_pair(s, s);
        }
        GX_UNEXPECTED;
    }();
    const auto target_aspect_ratio = get_target_aspect_ratio();
    const auto far_x = static_cast<double>(target_aspect_ratio) * fpn;
    const auto far_y = fpn;
    const auto nz = z * -static_cast<double>(near);
    const auto fz = z * -static_cast<double>(far);
    const auto nx = x * (static_cast<double>(target_aspect_ratio) * scale);
    const auto fx = x * far_x;
    const auto ny = y * scale;
    const auto fy = y * far_y;
    points[0] = location - fx + fy + fz;
    points[1] = location + fx + fy + fz;
    points[2] = location - fx - fy + fz;
    points[3] = location + fx - fy + fz;
    points[4] = location - nx + ny + nz;
    points[5] = location + nx + ny + nz;
    points[6] = location - nx - ny + nz;
    points[7] = location + nx - ny + nz;
}

void gearoenix::render::camera::Camera::set_view(const math::Mat4x4<float>& v)
{
    view = v;
    view_projection = projection * v;
}

void gearoenix::render::camera::Camera::set_customised_target_aspect_ratio(const float tar)
{
    customised_target_aspect_ratio = tar;
    update_projection();
}

void gearoenix::render::camera::Camera::disable_customised_target_aspect_ratio()
{
    customised_target_aspect_ratio = std::nullopt;
    update_projection();
}

float gearoenix::render::camera::Camera::get_target_aspect_ratio() const
{
    const auto td = customised_target_aspect_ratio.has_value() ? *customised_target_aspect_ratio : static_cast<float>(target.get_aspect_ratio());
    const auto clip = starting_clip_ending_clip.zw() - starting_clip_ending_clip.xy();
    return td * clip.x / clip.y;
}

void gearoenix::render::camera::Camera::set_projection_data(const ProjectionData p)
{
    projection_data = p;
    update_projection();
}

void gearoenix::render::camera::Camera::update_projection()
{
    const auto target_aspect_ratio = get_target_aspect_ratio();
    if (const auto* const p = std::get_if<PerspectiveProjectionData>(&projection_data)) {
        const auto s = 2.0f * near * tanf(p->field_of_view_y * 0.5f);
        projection = math::Mat4x4<float>::perspective(target_aspect_ratio * s, s, near, far);
    } else if (const auto* const o = std::get_if<OrthographicProjectionData>(&projection_data)) {
        projection = math::Mat4x4<float>::orthographic(target_aspect_ratio * o->scale, o->scale, near, far);
    } else {
        GX_UNEXPECTED;
    }
    view_projection = projection * view;

    if (debug_enabled) {
        create_debug_mesh(core::job::EndCaller([] { }));
    }
}

void gearoenix::render::camera::Camera::set_near(const float f)
{
    near = f;
    update_projection();
}

void gearoenix::render::camera::Camera::set_far(const float f)
{
    far = f;
    update_projection();
}

void gearoenix::render::camera::Camera::show_debug_gui(const core::ecs::World& w)
{
    if (ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        Component::show_debug_gui(w);
        bool input_changed = false;
        input_changed |= ImGui::InputFloat("Far", &far, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat("Near", &near, 0.01f, 1.0f, "%.3f");
        if (customised_target_aspect_ratio.has_value()) {
            input_changed |= ImGui::InputFloat("Customised Target Aspect Ratio", &*customised_target_aspect_ratio, 0.01f, 1.0f, "%.3f");
        }
        if (auto* const p = std::get_if<PerspectiveProjectionData>(&projection_data)) {
            input_changed |= ImGui::InputFloat("Field Of View Y", &p->field_of_view_y, 0.01f, 1.0f, "%.3f");
        } else if (auto* const o = std::get_if<OrthographicProjectionData>(&projection_data)) {
            input_changed |= ImGui::InputFloat("Scale", &o->scale, 0.01f, 1.0f, "%.3f");
        } else {
            GX_UNEXPECTED;
        }

        input_changed |= ImGui::Checkbox("Show debug mesh", &debug_enabled);
        if (input_changed) {
            update_projection();
        }
        exposure.show_debug_gui();
        if (bloom_data.has_value()) {
            bloom_data->show_debug_data();
        }
        ImGui::TreePop();
    }
}

void gearoenix::render::camera::Camera::enable_debug_mesh(core::job::EndCaller<>&& end)
{
    if (!debug_enabled) {
        debug_enabled = true;
        create_debug_mesh(std::move(end));
    }
}

void gearoenix::render::camera::Camera::disable_debug_mesh()
{
    debug_enabled = false;
}

gearoenix::math::Ray3<double> gearoenix::render::camera::Camera::generate_ray(
    const physics::Transformation& transform, const math::Vec2<double>& normalised_point) const
{
    const auto [scale, is_perspective] = [&] {
        if (const auto* const p = std::get_if<PerspectiveProjectionData>(&projection_data)) {
            return std::make_pair(static_cast<double>(2.0f * near * tanf(p->field_of_view_y * 0.5f)), true);
        }
        if (const auto* const o = std::get_if<OrthographicProjectionData>(&projection_data)) {
            return std::make_pair(static_cast<double>(o->scale), false);
        }
        GX_UNEXPECTED;
    }();
    const auto near_plane_point = normalised_point * scale;
    const auto direction = (transform.get_x_axis() * near_plane_point.x) + (transform.get_y_axis() * near_plane_point.y) + (transform.get_z_axis() * static_cast<double>(-near));
    const auto origin = transform.get_local_location() + direction;
    return { origin, is_perspective ? direction.normalised() : -transform.get_z_axis() };
}

void gearoenix::render::camera::Camera::set_customised_target(std::shared_ptr<texture::Target>&& t)
{
    target.set_customised(std::move(t));
    bloom_data = std::nullopt;
    update_projection();
}

void gearoenix::render::camera::Camera::create_debug_mesh(core::job::EndCaller<>&& end)
{
    e.get_material_manager()->get_unlit(
        "dummy",
        core::job::EndCallerShared<material::Unlit>([this, self = camera_self.lock(), end = std::move(end)](std::shared_ptr<material::Unlit>&& material) mutable {
            if (nullptr == self) {
                return;
            }
            std::string mesh_name = name + "-camera-debug-mesh-ptr" + std::to_string(reinterpret_cast<std::size_t>(this));
            (void)e.get_mesh_manager()->remove_if_exist(mesh_name);
            std::array<math::Vec3<double>, 8> points;
            generate_frustum_points(
                math::Vec3(0.0),
                math::X3D<double>,
                math::Y3D<double>,
                math::Z3D<double>,
                points);
            std::vector<PbrVertex> vertices(points.size());
            for (std::size_t i = 0; i < points.size(); ++i) {
                vertices[i].position = math::Vec3<float>(points[i]);
            }
            std::vector<std::uint32_t> indices {
                0, 1, 0, 2, 1, 3, 2, 3, // far
                0, 4, 1, 5, 2, 6, 3, 7, // edges
                4, 5, 4, 6, 5, 7, 6, 7, // near
            };
            e.get_mesh_manager()->build(
                std::move(mesh_name),
                std::move(vertices),
                std::move(indices),
                std::move(material),
                core::job::EndCallerShared<mesh::Mesh>([this, self = std::move(self), end = std::move(end)](std::shared_ptr<mesh::Mesh>&& m) {
                    debug_mesh = std::move(m);
                    (void)self;
                    (void)end;
                }));
        }));
}

void gearoenix::render::camera::Camera::disable_bloom()
{
    bloom_data = std::nullopt;
}

void gearoenix::render::camera::Camera::enable_bloom()
{
    if (bloom_data.has_value()) {
        return;
    }
    bloom_data.emplace();
}

void gearoenix::render::camera::Camera::update_bloom()
{
}

void gearoenix::render::camera::Camera::update_target(core::job::EndCaller<>&& end)
{
    if (!target.is_default()) {
        return;
    }
    e.get_texture_manager()->create_default_camera_render_target(
        name,
        core::job::EndCaller<texture::DefaultCameraTargets>([w = camera_self, end = std::move(end)](texture::DefaultCameraTargets&& t) mutable {
            auto s = w.lock();
            if (nullptr == s) {
                return;
            }
            s->target = Target(std::move(t));
            s->update_projection();
            s->update_bloom();
        }));
}
