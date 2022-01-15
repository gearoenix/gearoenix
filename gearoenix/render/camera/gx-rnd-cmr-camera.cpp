#include "gx-rnd-cmr-camera.hpp"

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
    , projection(Projection::Perspective == projection_type ? math::Mat4x4<float>::perspective(target_aspect_ratio, 1.0f, near, far) : math::Mat4x4<float>::orthographic(target_aspect_ratio, 1.0f, near, far))
    , view_projection(projection)
{
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
    set_projection(Projection::Perspective == projection_type ? math::Mat4x4<float>::perspective(target_aspect_ratio * scale, scale, near, far) : math::Mat4x4<float>::orthographic(target_aspect_ratio * scale, scale, near, far));
}

void gearoenix::render::camera::Camera::set_projection(const math::Mat4x4<float>& p) noexcept
{
    projection = p;
    view_projection = p * view;
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

//    auto* const platform_application = render_engine->get_platform_application();
//    uniform.aspect_ratio = static_cast<float>(platform_application->get_event_engine()->get_window_ratio());
//    uniform.clip_width = static_cast<float>(platform_application->get_event_engine()->get_window_width());
//    uniform.clip_height = static_cast<float>(platform_application->get_event_engine()->get_window_height());
//    set_target(render_engine->get_main_render_target().get());
//    platform_application->get_event_engine()->add_listener(core::event::Id::PlatformWindowSizeChange, 1.0f, this);
//}
//
// gearoenix::render::camera::Camera::Camera(const core::Id my_id, std::string name, engine::Engine* const e) noexcept
//    : GX_CAMERA_INIT
//{
//    initialize();
//}
//
// gearoenix::render::camera::Camera::Camera(
//    const core::Id my_id,
//    std::string name,
//    platform::stream::Stream* const f,
//    engine::Engine* const e) noexcept
//    : GX_CAMERA_INIT
//{
//    initialize();
//    uniform.position.read(f);
//    math::Quat<float> q;
//    f->read(q.w);
//    f->read(q.x);
//    f->read(q.y);
//    f->read(q.z);
//    uniform.near = -f->read<float>();
//    uniform.far = -f->read<float>();
//    const math::Mat4x4 r = math::Mat4x4<double>(q.to_mat());
//    transformation->set_x_axis((r * math::Vec4(transformation->get_x_axis(), 0.0)).xyz());
//    transformation->set_y_axis((r * math::Vec4(transformation->get_y_axis(), 0.0)).xyz());
//    transformation->set_z_axis((r * math::Vec4(transformation->get_z_axis(), 0.0)).xyz());
//    uniform.inverted_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
//    GXLOGD("Position: " << uniform.position)
//    GXLOGD("Quaternion: " << q)
//    GXLOGD("Near: " << uniform.near)
//    reinterpret_cast<Transformation*>(transformation.get())->update_location();
//}
//
// void gearoenix::render::camera::Camera::config_target() const noexcept
//{
//    target->set_clipping_width(static_cast<unsigned int>(uniform.clip_width));
//    target->set_clipping_height(static_cast<unsigned int>(uniform.clip_height));
//    target->set_clipping_starting_x(static_cast<unsigned int>(uniform.clip_start_x));
//    target->set_clipping_starting_y(static_cast<unsigned int>(uniform.clip_start_y));
//}
//
// gearoenix::render::camera::Camera::~Camera() noexcept
//{
//    auto* const event_engine = render_engine->get_platform_application()->get_event_engine();
//    if (nullptr != event_engine)
//        event_engine->remove_listener(core::event::Id::PlatformWindowSizeChange, this);
//}
//
// void gearoenix::render::camera::Camera::set_far(const float f) noexcept
//{
//    uniform.far = -std::abs(f);
//    reinterpret_cast<Transformation*>(transformation.get())->update_projection();
//}
//
// float gearoenix::render::camera::Camera::get_far() const noexcept
//{
//    return std::abs(uniform.far);
//}
//
// void gearoenix::render::camera::Camera::set_near(const float f) noexcept
//{
//    uniform.near = -std::abs(f);
//    reinterpret_cast<Transformation*>(transformation.get())->update_projection();
//}
//
// float gearoenix::render::camera::Camera::get_near() const noexcept
//{
//    return std::abs(uniform.near);
//}
//
// void gearoenix::render::camera::Camera::set_gamma_correction_factor(const float f) noexcept
//{
//    uniform.gamma_correction = f;
//}
//
// void gearoenix::render::camera::Camera::set_target(const texture::Target* const t) noexcept
//{
//    target = std::unique_ptr<texture::Target>(t->clone());
//    config_target();
//}
//
// void gearoenix::render::camera::Camera::set_cascaded_shadow_frustum_partitions_count(const std::size_t c) noexcept
//{
//    cascaded_shadow_frustum_partitions_count = c;
//    transformation->update_frustum();
//}
//
// void gearoenix::render::camera::Camera::update() noexcept
//{
//    uniform_buffers->update(uniform);
//
//    seen_static_opaque_meshes.clear();
//    seen_static_transparent_meshes.clear();
//    seen_dynamic_opaque_meshes.clear();
//    seen_dynamic_transparent_meshes.clear();
//    seen_transparent_meshes.clear();
//
//    cascades.refresh();
//}
//
// void gearoenix::render::camera::Camera::set_aspects(const unsigned int w, const unsigned h) noexcept
//{
//    uniform.clip_width = static_cast<float>(w);
//    uniform.clip_height = static_cast<float>(h);
//    uniform.aspect_ratio = uniform.clip_width / uniform.clip_height;
//    config_target();
//}
//
// void gearoenix::render::camera::Camera::check_static_models(const physics::accelerator::Bvh* const bvh) noexcept
//{
//    const std::function<void(physics::collider::Collider* const)> collided = [this](physics::collider::Collider* const cld) noexcept {
//        auto* const m = cld->get_parent();
//        m->update();
//        const auto& model_meshes = m->get_meshes();
//        for (const auto& model_mesh : model_meshes) {
//            auto* const msh = model_mesh.second.get();
//            auto* const mat = msh->get_mat().get();
//            const auto dis = get_distance(cld->get_location());
//            if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
//                seen_static_transparent_meshes.emplace_back(-dis, mat->get_material_type(), m, msh);
//            } else {
//                seen_static_opaque_meshes.emplace_back(dis, mat->get_material_type(), m, msh);
//            }
//        }
//    };
//    bvh->call_on_intersecting(frustum_collider.get(), collided);
//    std::sort(seen_static_transparent_meshes.begin(), seen_static_transparent_meshes.end());
//    std::sort(seen_static_opaque_meshes.begin(), seen_static_opaque_meshes.end());
//}
//
// void gearoenix::render::camera::Camera::check_dynamic_models(const physics::accelerator::Bvh* const bvh) noexcept
//{
//    const std::function<void(physics::collider::Collider* const)> collided = [this](physics::collider::Collider* const cld) noexcept {
//        auto* const m = cld->get_parent();
//        m->update();
//        const auto& model_meshes = m->get_meshes();
//        for (const auto& model_mesh : model_meshes) {
//            auto* const msh = model_mesh.second.get();
//            auto* const mat = msh->get_mat().get();
//            const auto dis = get_distance(cld->get_location());
//            if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
//                seen_dynamic_transparent_meshes.emplace_back(-dis, mat->get_material_type(), m, msh);
//            } else {
//                seen_dynamic_opaque_meshes.emplace_back(dis, mat->get_material_type(), m, msh);
//            }
//        }
//    };
//    bvh->call_on_intersecting(frustum_collider.get(), collided);
//    std::sort(seen_dynamic_transparent_meshes.begin(), seen_dynamic_transparent_meshes.end());
//    std::sort(seen_dynamic_opaque_meshes.begin(), seen_dynamic_opaque_meshes.end());
//}
//
// void gearoenix::render::camera::Camera::cascade_shadow(const light::Directional* const l) noexcept
//{
//    auto* const cascade_info = cascades.get_next([this] {
//        return new light::CascadeInfo(render_engine);
//    });
//    cascade_info->set_source(l);
//    const auto& dir = l->get_direction();
//    const auto dot = std::abs(dir.dot(math::Vec3(0.0, 1.0, 0.0))) - 1.0;
//    const auto& up = GX_IS_ZERO(dot) ? math::Vec3(0.0, 0.0, 1.0) : math::Vec3(0.0, 1.0, 0.0);
//    const auto view = math::Mat4x4<double>::look_at(math::Vec3<double>(), dir, up);
//    cascade_info->update(view, cascaded_shadow_frustum_partitions);
//}
//
// void gearoenix::render::camera::Camera::merge_seen_meshes() noexcept
//{
//    std::merge(seen_dynamic_transparent_meshes.begin(), seen_dynamic_transparent_meshes.end(),
//        seen_static_transparent_meshes.begin(), seen_static_transparent_meshes.end(),
//        std::back_inserter(seen_transparent_meshes));
//}
//
// bool gearoenix::render::camera::Camera::on_event(const core::event::Data& d) noexcept
//{
//    const auto* platform_application = render_engine->get_platform_application();
//    switch (d.get_source()) {
//    case core::event::Id::PlatformWindowSizeChange:
//        if (target->get_attachments().empty())
//            set_aspects(
//                platform_application->get_event_engine()->get_window_width(),
//                platform_application->get_event_engine()->get_window_height());
//        return false;
//    default:
//        GXLOGF("Unexpected event received this is a fatal bug.")
//    }
//}
