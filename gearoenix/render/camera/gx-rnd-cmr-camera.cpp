#include "gx-rnd-cmr-camera.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../buffer/gx-rnd-buf-framed-uniform.hpp"
#include "../light/gx-rnd-lt-cascade-info.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../material/gx-rnd-mat-material.hpp"
#include "../model/gx-rnd-mdl-mesh.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include "gx-rnd-cmr-transformation.hpp"
#include <algorithm>

#define GX_CAMERA_INIT                                                                                                                                                \
    core::asset::Asset(my_id, core::asset::Type::Camera, std::move(name)),                                                                                            \
        frustum_collider(new physics::collider::Frustum()),                                                                                                           \
        uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e)),                                                                                               \
        cascaded_shadow_frustum_partitions(static_cast<std::size_t>(e->get_system_application()->get_configuration().render_config.get_shadow_cascades_count()) + 1), \
        transformation(new Transformation(&uniform, frustum_collider.get(), &cascaded_shadow_frustum_partitions)),                                                    \
        render_engine(e)

void gearoenix::render::camera::Camera::initialize() noexcept
{
    auto* const sys_app = render_engine->get_system_application();
    uniform.aspect_ratio = static_cast<float>(sys_app->get_event_engine()->get_window_ratio());
    uniform.clip_width = static_cast<float>(sys_app->get_event_engine()->get_window_width());
    uniform.clip_height = static_cast<float>(sys_app->get_event_engine()->get_window_height());
    set_target(render_engine->get_main_render_target().get());
    sys_app->get_event_engine()->add_listener(core::event::Id::SystemWindowSizeChange, 1.0f, this);
}

gearoenix::render::camera::Camera::Camera(const core::Id my_id, std::string name, engine::Engine* const e) noexcept
    : GX_CAMERA_INIT
{
    initialize();
}

gearoenix::render::camera::Camera::Camera(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : GX_CAMERA_INIT
{
    initialize();
    uniform.position.read(f);
    math::Quat<float> q;
    f->read(q.w);
    f->read(q.x);
    f->read(q.y);
    f->read(q.z);
    uniform.near = -f->read<float>();
    uniform.far = -f->read<float>();
    const math::Mat4x4 r = math::Mat4x4<double>(q.to_mat());
    transformation->set_x_axis((r * math::Vec4(transformation->get_x_axis(), 0.0)).xyz());
    transformation->set_y_axis((r * math::Vec4(transformation->get_y_axis(), 0.0)).xyz());
    transformation->set_z_axis((r * math::Vec4(transformation->get_z_axis(), 0.0)).xyz());
    uniform.inverted_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    GXLOGD("Position: " << uniform.position)
    GXLOGD("Quaternion: " << q)
    GXLOGD("Near: " << uniform.near)
    reinterpret_cast<Transformation*>(transformation.get())->update_location();
}

void gearoenix::render::camera::Camera::config_target() const noexcept
{
    target->set_clipping_width(static_cast<unsigned int>(uniform.clip_width));
    target->set_clipping_height(static_cast<unsigned int>(uniform.clip_height));
    target->set_clipping_starting_x(static_cast<unsigned int>(uniform.clip_start_x));
    target->set_clipping_starting_y(static_cast<unsigned int>(uniform.clip_start_y));
}

gearoenix::render::camera::Camera::~Camera() noexcept
{
    auto* const event_engine = render_engine->get_system_application()->get_event_engine();
    if (nullptr != event_engine)
        event_engine->remove_listener(core::event::Id::SystemWindowSizeChange, this);
}

void gearoenix::render::camera::Camera::set_far(const float f) noexcept
{
    uniform.far = -std::abs(f);
    reinterpret_cast<Transformation*>(transformation.get())->update_projection();
}

float gearoenix::render::camera::Camera::get_far() const noexcept
{
    return std::abs(uniform.far);
}

void gearoenix::render::camera::Camera::set_near(const float f) noexcept
{
    uniform.near = -std::abs(f);
    reinterpret_cast<Transformation*>(transformation.get())->update_projection();
}

float gearoenix::render::camera::Camera::get_near() const noexcept
{
    return std::abs(uniform.near);
}

void gearoenix::render::camera::Camera::set_gamma_correction_factor(const float f) noexcept
{
    uniform.gamma_correction = f;
}

void gearoenix::render::camera::Camera::set_target(const texture::Target* const t) noexcept
{
    target = std::unique_ptr<texture::Target>(t->clone());
    config_target();
}

void gearoenix::render::camera::Camera::set_cascaded_shadow_frustum_partitions_count(const std::size_t c) noexcept
{
    cascaded_shadow_frustum_partitions_count = c;
    transformation->update_frustum();
}

void gearoenix::render::camera::Camera::update() noexcept
{
    uniform_buffers->update(uniform);

    seen_static_opaque_meshes.clear();
    seen_static_transparent_meshes.clear();
    seen_dynamic_opaque_meshes.clear();
    seen_dynamic_transparent_meshes.clear();
    seen_transparent_meshes.clear();

    cascades.refresh();
}

void gearoenix::render::camera::Camera::set_aspects(const unsigned int w, const unsigned h) noexcept
{
    uniform.clip_width = static_cast<float>(w);
    uniform.clip_height = static_cast<float>(h);
    uniform.aspect_ratio = uniform.clip_width / uniform.clip_height;
    config_target();
}

void gearoenix::render::camera::Camera::check_static_models(const physics::accelerator::Bvh* const bvh) noexcept
{
    const std::function<void(physics::collider::Collider* const)> collided = [this](physics::collider::Collider* const cld) noexcept {
        auto* const m = cld->get_parent();
        m->update();
        const auto& model_meshes = m->get_meshes();
        for (const auto& model_mesh : model_meshes) {
            auto* const msh = model_mesh.second.get();
            auto* const mat = msh->get_mat().get();
            const auto dis = get_distance(cld->get_location());
            if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
                seen_static_transparent_meshes.emplace_back(-dis, mat->get_material_type(), m, msh);
            } else {
                seen_static_opaque_meshes.emplace_back(dis, mat->get_material_type(), m, msh);
            }
        }
    };
    bvh->call_on_intersecting(frustum_collider.get(), collided);
    std::sort(seen_static_transparent_meshes.begin(), seen_static_transparent_meshes.end());
    std::sort(seen_static_opaque_meshes.begin(), seen_static_opaque_meshes.end());
}

void gearoenix::render::camera::Camera::check_dynamic_models(const physics::accelerator::Bvh* const bvh) noexcept
{
    const std::function<void(physics::collider::Collider* const)> collided = [this](physics::collider::Collider* const cld) noexcept {
        auto* const m = cld->get_parent();
        m->update();
        const auto& model_meshes = m->get_meshes();
        for (const auto& model_mesh : model_meshes) {
            auto* const msh = model_mesh.second.get();
            auto* const mat = msh->get_mat().get();
            const auto dis = get_distance(cld->get_location());
            if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
                seen_dynamic_transparent_meshes.emplace_back(-dis, mat->get_material_type(), m, msh);
            } else {
                seen_dynamic_opaque_meshes.emplace_back(dis, mat->get_material_type(), m, msh);
            }
        }
    };
    bvh->call_on_intersecting(frustum_collider.get(), collided);
    std::sort(seen_dynamic_transparent_meshes.begin(), seen_dynamic_transparent_meshes.end());
    std::sort(seen_dynamic_opaque_meshes.begin(), seen_dynamic_opaque_meshes.end());
}

void gearoenix::render::camera::Camera::cascade_shadow(const light::Directional* const l) noexcept
{
    auto* const cascade_info = cascades.get_next([this] {
        return new light::CascadeInfo(render_engine);
    });
    cascade_info->set_source(l);
    const auto& dir = l->get_direction();
    const auto dot = std::abs(dir.dot(math::Vec3(0.0, 1.0, 0.0))) - 1.0;
    const auto& up = GX_IS_ZERO(dot) ? math::Vec3(0.0, 0.0, 1.0) : math::Vec3(0.0, 1.0, 0.0);
    const auto view = math::Mat4x4<double>::look_at(math::Vec3<double>(), dir, up);
    cascade_info->update(view, cascaded_shadow_frustum_partitions);
}

void gearoenix::render::camera::Camera::merge_seen_meshes() noexcept
{
    std::merge(seen_dynamic_transparent_meshes.begin(), seen_dynamic_transparent_meshes.end(),
        seen_static_transparent_meshes.begin(), seen_static_transparent_meshes.end(),
        std::back_inserter(seen_transparent_meshes));
}

bool gearoenix::render::camera::Camera::on_event(const core::event::Data& d) noexcept
{
    const auto* sys_app = render_engine->get_system_application();
    switch (d.get_source()) {
    case core::event::Id::SystemWindowSizeChange:
        if (target->get_attachments().empty())
            set_aspects(
                sys_app->get_event_engine()->get_window_width(),
                sys_app->get_event_engine()->get_window_height());
        return false;
    default:
        GXLOGF("Unexpected event received this is a fatal bug.")
    }
}
