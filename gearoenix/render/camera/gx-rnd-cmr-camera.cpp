#include "gx-rnd-cmr-camera.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include <random>

namespace {
thread_local std::random_device rd;
thread_local std::uniform_real_distribution urd(0.5f, 1.0f);
thread_local std::default_random_engine re(rd());
}

gearoenix::render::camera::Camera::Camera(core::ecs::Entity* const entity, const core::object_type_index_t final_type, const std::string& name, Target&& target, std::shared_ptr<physics::Transformation>&& transform)
    : Component(entity, final_type, std::string(name))
    , starting_clip_ending_clip(0.0f, 0.0f, 1.0f, 1.0f)
    , target(std::move(target))
    , debug_colour {
        urd(re),
        urd(re),
        urd(re),
        urd(re),
    }
    , transform(std::move(transform))
    , render_pass_name("render-shadows for camera " + object_name)
{
    if (this->target.is_customised()) {
        exposure.disable();
    }
}

gearoenix::render::camera::Camera::~Camera()
{
    RuntimeConfiguration::get().get_runtime_resolution().remove_observer(resolution_cfg_observer);
}

void gearoenix::render::camera::Camera::initialise()
{
    if (0 == resolution_cfg_observer) {
        resolution_cfg_observer = RuntimeConfiguration::get().get_runtime_resolution().add_observer(
            [c = object_self](const Resolution&) {
                const auto cam = std::static_pointer_cast<Camera>(std::move(c.lock()));
                if (nullptr == cam) {
                    return false;
                }
                cam->update_target(core::job::EndCaller([] { }));
                return true;
            });
    }
}

void gearoenix::render::camera::Camera::write(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const
{
    starting_clip_ending_clip.write(*s);
    if (customised_target_aspect_ratio.has_value()) {
        s->write_fail_debug(true);
        s->write_fail_debug(*customised_target_aspect_ratio);
    } else {
        s->write_fail_debug(false);
    }
    s->write_fail_debug(flag);
    s->write_fail_debug(far);
    s->write_fail_debug(near);
    colour_tuning.write(*s);
    projection_data.write(*s);
    s->write_fail_debug(layer);
    s->write_fail_debug(usage);
    s->write_fail_debug(bloom_data.has_value());
    if (bloom_data.has_value()) {
        bloom_data->get_scatter_clamp_max_threshold_threshold_knee().write(*s);
    }
    exposure.write(*s);
    target.write(std::move(s), std::move(end));
}

void gearoenix::render::camera::Camera::read(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end)
{
    starting_clip_ending_clip.read(*s);
    if (s->read<bool>()) {
        customised_target_aspect_ratio.emplace();
        s->read(*customised_target_aspect_ratio);
    }
    s->read(flag);
    s->read(far);
    s->read(near);
    colour_tuning.read(*s);
    projection_data.read(*s);
    s->read(layer);
    s->read(usage);
    if (s->read<bool>()) {
        bloom_data.emplace();
        bloom_data->get_scatter_clamp_max_threshold_threshold_knee().read(*s);
    }
    exposure.read(*s);
    core::ecs::World::get().resolve([this, self = object_self.lock(), end] {
        (void)self;
        (void)end;
        transform = entity->get_component_shared_ptr<physics::Transformation>();
        return transform == nullptr;
    });
    Target::read(std::move(s), core::job::EndCaller<Target>([this, s = object_self.lock(), end = std::move(end)](Target&& t) mutable {
        (void)s;
        target = std::move(t);
        update_target(core::job::EndCaller([end = std::move(end)] { (void)end; }));
    }));
}

void gearoenix::render::camera::Camera::generate_frustum_points(
    const math::Vec3<double>& location,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    std::array<math::Vec3<double>, 8>& points) const
{
    const auto [scale, fpn] = [&] {
        if (projection_data.is_perspective()) {
            const auto [field_of_view_y] = projection_data.get_perspective();
            const auto s = static_cast<double>(near * tanf(field_of_view_y * 0.5f));
            return std::make_pair(s, (static_cast<double>(far) + static_cast<double>(near)) * s / static_cast<double>(near));
        }
        if (projection_data.is_orthographic()) {
            const auto& [scale] = projection_data.get_orthographic();
            const auto s = static_cast<double>(scale * 0.5f);
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

bool gearoenix::render::camera::Camera::is_perspective() const
{
    return projection_data.is_perspective();
}

bool gearoenix::render::camera::Camera::is_orthographic() const
{
    return projection_data.is_orthographic();
}

void gearoenix::render::camera::Camera::update_projection()
{
    const auto target_aspect_ratio = get_target_aspect_ratio();
    if (projection_data.is_perspective()) {
        const auto& [field_of_view_y] = projection_data.get_perspective();
        const auto s = 2.0f * near * tanf(field_of_view_y * 0.5f);
        projection = math::Mat4x4<float>::perspective(target_aspect_ratio * s, s, near, far);
    } else if (projection_data.is_orthographic()) {
        const auto& [scale] = projection_data.get_orthographic();
        projection = math::Mat4x4<float>::orthographic(target_aspect_ratio * scale, scale, near, far);
    } else {
        GX_UNEXPECTED;
    }
    view_projection = projection * view;
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

void gearoenix::render::camera::Camera::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        Component::show_debug_gui();
        bool input_changed = false;
        input_changed |= ImGui::InputFloat("Far", &far, 0.01f, 1.0f, "%.3f");
        input_changed |= ImGui::InputFloat("Near", &near, 0.01f, 1.0f, "%.3f");
        if (customised_target_aspect_ratio.has_value()) {
            input_changed |= ImGui::InputFloat("Customised Target Aspect Ratio", &*customised_target_aspect_ratio, 0.01f, 1.0f, "%.3f");
        }
        if (projection_data.is_perspective()) {
            auto& [field_of_view_y] = projection_data.get_perspective();
            input_changed |= ImGui::InputFloat("Field Of View Y", &field_of_view_y, 0.01f, 1.0f, "%.3f");
        } else if (projection_data.is_orthographic()) {
            auto& [scale] = projection_data.get_orthographic();
            input_changed |= ImGui::InputFloat("Scale", &scale, 0.01f, 1.0f, "%.3f");
        } else {
            GX_UNEXPECTED;
        }

        if (input_changed) {
            update_projection();
        }
        exposure.show_debug_gui();
        if (bloom_data.has_value()) {
            bloom_data->show_debug_data();
        }
    });
}

gearoenix::math::Ray3<double> gearoenix::render::camera::Camera::generate_ray(const math::Vec2<double>& normalised_point) const
{
    const auto [scale, is_perspective] = [&] {
        if (projection_data.is_perspective()) {
            const auto [field_of_view_y] = projection_data.get_perspective();
            return std::make_pair(static_cast<double>(2.0f * near * tanf(field_of_view_y * 0.5f)), true);
        }
        if (projection_data.is_orthographic()) {
            const auto [scale] = projection_data.get_orthographic();
            return std::make_pair(static_cast<double>(scale), false);
        }
        GX_UNEXPECTED;
    }();
    const auto near_plane_point = math::Vec3(normalised_point * scale, static_cast<double>(-near));
    const auto direction = transform->get_rotation_matrix() * near_plane_point;
    const auto origin = transform->get_local_position() + direction;
    return { origin, is_perspective ? direction.normalised() : -transform->get_z_axis() };
}

void gearoenix::render::camera::Camera::set_customised_target(std::shared_ptr<texture::Target>&& t)
{
    target.set_customised(std::move(t));
    bloom_data = std::nullopt;
    update_projection();
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
    texture::Manager::get().create_default_camera_render_target(
        object_name,
        core::job::EndCaller<texture::DefaultCameraTargets>([this, w = object_self, end = std::move(end)](texture::DefaultCameraTargets&& t) mutable {
            const auto s = w.lock();
            if (nullptr == s) {
                return;
            }
            target = Target(std::move(t));
            update_projection();
            update_bloom();
            (void)s;
        }));
}
