#include "rnd-cmr-camera.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../math/math-projector-frustum.hpp"
#include "../../physics/accelerator/phs-acc-bvh.hpp"
#include "../../physics/collider/phs-cld-frustum.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../light/rnd-lt-cascade-info.hpp"
#include "../light/rnd-lt-directional.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../texture/rnd-txt-target.hpp"
#include "rnd-cmr-transformation.hpp"

#define GX_CAMERA_INIT \
    core::asset::Asset(my_id, core::asset::Type::Camera), frustum_collider(new physics::collider::Frustum()), uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e)), cascaded_shadow_frustum_partitions(static_cast<std::size_t>(e->get_system_application()->get_configuration().render_config.shadow_cascades_count) + 1), transformation(new Transformation(&uniform, frustum_collider.get(), &cascaded_shadow_frustum_partitions)), e(e)

void gearoenix::render::camera::Camera::initialize() noexcept
{
    auto* const sys_app = e->get_system_application();
    uniform.aspect_ratio = sys_app->get_window_ratio();
    uniform.clip_width = static_cast<core::Real>(sys_app->get_window_width());
    uniform.clip_height = static_cast<core::Real>(sys_app->get_window_height());
    set_target(e->get_main_render_target().get());
    sys_app->get_event_engine()->add_listener(core::event::Id::SystemWindowSizeChange, 1.0f, this);
}

gearoenix::render::camera::Camera::Camera(const core::Id my_id, engine::Engine* const e) noexcept
    : GX_CAMERA_INIT
{
    initialize();
}

gearoenix::render::camera::Camera::Camera(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : GX_CAMERA_INIT
{
    initialize();
    uniform.position.read(f);
    math::Quat q;
    f->read(q.w);
    f->read(q.x);
    f->read(q.y);
    f->read(q.z);
    uniform.near = -f->read<core::Real>();
    uniform.far = -f->read<core::Real>();
    const math::Mat4x4 r = q.to_mat();
    transformation->set_x_axis((r * math::Vec4(transformation->get_x_axis(), 0.0f)).xyz());
    transformation->set_y_axis((r * math::Vec4(transformation->get_y_axis(), 0.0f)).xyz());
    transformation->set_z_axis((r * math::Vec4(transformation->get_z_axis(), 0.0f)).xyz());
    uniform.inversed_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    GXLOGD("Position: " << uniform.position)
    GXLOGD("Quaternion: " << q)
    GXLOGD("Near: " << uniform.near)
    reinterpret_cast<Transformation*>(transformation.get())->update_location();
}

void gearoenix::render::camera::Camera::config_target() const noexcept
{
    target->set_clipping_width(uniform.clip_width);
    target->set_clipping_height(uniform.clip_height);
    target->set_clipping_starting_x(uniform.clip_start_x);
    target->set_clipping_starting_y(uniform.clip_start_y);
}

gearoenix::render::camera::Camera::~Camera() noexcept
{
    e->get_system_application()->get_event_engine()->remove_listener(core::event::Id::SystemWindowSizeChange, this);
}

void gearoenix::render::camera::Camera::set_far(const core::Real f) noexcept
{
    uniform.far = -f;
    reinterpret_cast<Transformation*>(transformation.get())->update_projection();
}

void gearoenix::render::camera::Camera::set_target(const texture::Target* const t) noexcept
{
    target = std::unique_ptr<texture::Target>(t->clone());
    config_target();
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

void gearoenix::render::camera::Camera::set_aspect_ratio(const gearoenix::core::Real ratio) noexcept
{
    uniform.aspect_ratio = ratio;
}

void gearoenix::render::camera::Camera::check_static_models(const physics::accelerator::Bvh* const bvh) noexcept
{
    const std::function<void(physics::collider::Collider* const cld)> collided = [this](physics::collider::Collider* const cld) noexcept {
        auto* const m = cld->get_parent();
        m->update();
        const auto& model_meshes = m->get_meshes();
        for (const auto& model_mesh : model_meshes) {
            auto* const msh = model_mesh.second.get();
            auto* const mat = msh->get_mat().get();
            if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
                seen_static_transparent_meshes.emplace_back(get_distance(cld->get_location()), mat->get_material_type(), m, msh);
            } else {
                seen_static_opaque_meshes.emplace_back(mat->get_material_type(), m, msh);
            }
        }
    };
    bvh->call_on_intersecting(frustum_collider.get(), collided);
    std::sort(seen_static_transparent_meshes.begin(), seen_static_transparent_meshes.end());
}

void gearoenix::render::camera::Camera::check_dynamic_models(const physics::accelerator::Bvh* const bvh) noexcept
{
    const std::function<void(physics::collider::Collider* const cld)> collided = [this](physics::collider::Collider* const cld) noexcept {
        auto* const m = cld->get_parent();
        m->update();
        const auto& model_meshes = m->get_meshes();
        for (const auto& model_mesh : model_meshes) {
            auto* const msh = model_mesh.second.get();
            auto* const mat = msh->get_mat().get();
            if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
                seen_dynamic_transparent_meshes.emplace_back(get_distance(cld->get_location()), mat->get_material_type(), m, msh);
            } else {
                seen_dynamic_opaque_meshes.emplace_back(mat->get_material_type(), m, msh);
            }
        }
    };
    bvh->call_on_intersecting(frustum_collider.get(), collided);
    std::sort(seen_dynamic_transparent_meshes.begin(), seen_dynamic_transparent_meshes.end());
}

void gearoenix::render::camera::Camera::cascade_shadow(const light::Directional* const l) noexcept
{
    auto* const cascade_info = cascades.get_next([this] {
        return new light::CascadeInfo(e);
    });
    cascade_info->set_source(l);
    const auto& dir = l->get_direction();
    const auto dot = std::abs(dir.dot(math::Vec3(0.0f, 1.0f, 0.0f))) - 1.0f;
    const auto& up = GX_IS_ZERO(dot) ? math::Vec3::Z : math::Vec3::Y;
    const auto view = math::Mat4x4::look_at(math::Vec3(), dir, up);
    cascade_info->update(view, cascaded_shadow_frustum_partitions);
}

void gearoenix::render::camera::Camera::merge_seen_meshes() noexcept {
    std::merge(seen_dynamic_transparent_meshes.begin(), seen_dynamic_transparent_meshes.end(),
               seen_static_transparent_meshes.begin(), seen_static_transparent_meshes.end(),
               std::back_inserter(seen_transparent_meshes));
}

bool gearoenix::render::camera::Camera::on_event(const core::event::Data& d) noexcept
{
    const auto* sys_app = e->get_system_application();
    switch (d.source) {
    case core::event::Id::SystemWindowSizeChange:
        uniform.clip_width = static_cast<core::Real>(sys_app->get_window_width());
        uniform.clip_height = static_cast<core::Real>(sys_app->get_window_height());
        set_aspect_ratio(sys_app->get_window_ratio());
        config_target();
        return false;
    default:
        GXLOGF("Unexpected event received this is a fatal bug.")
    }
}
