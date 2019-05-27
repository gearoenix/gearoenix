#include "rnd-cmr-perspective.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-configuration.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-configuration.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-transformation.hpp"
#include "rnd-cmr-uniform.hpp"
#include <cmath>

void gearoenix::render::camera::Perspective::update_fovy() noexcept
{
    fovy = static_cast<core::Real>(std::atan(static_cast<double>(tany))) * 2.0f;
    uniform->projection = math::Mat4x4::perspective(
        tanx * -uniform->near * 2.0f,
        tany * -uniform->near * 2.0f,
        -uniform->near,
        -uniform->far);
    uniform->uniform_projection = math::Mat4x4(
                                      0.5f, 0.0f, 0.0f, 0.0f,
                                      0.0f, 0.5f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f,
                                      0.5f, 0.5f, 0.0f, 1.0f)
        * uniform->projection;
    lambda = static_cast<core::Real>(
                 std::sin(static_cast<double>(fovx) * 0.5) + std::sin(static_cast<double>(fovy) * 0.5))
        * 0.5f;
    transformation->update_view_projection();
}

void gearoenix::render::camera::Perspective::update_cascades() noexcept
{
    const system::Configuration& sys_conf = e->get_system_application()->get_configuration();
    const engine::Configuration& eng_conf = sys_conf.render_config;
    const std::size_t sections_count = eng_conf.shadow_cascades_count;
    if (cascaded_shadow_frustum_partitions->size() != sections_count + 1)
        cascaded_shadow_frustum_partitions->resize(sections_count + 1);

    const math::Vec3 xtanx = uniform->x * tanx;
    const math::Vec3 ytany = uniform->y * tany;

    math::Vec3 x = xtanx * -uniform->near;
    math::Vec3 y = ytany * -uniform->near;
    math::Vec3 z = uniform->position + (uniform->z * uniform->near);

    math::Vec3 zmx;
    math::Vec3 zpx;

#define GX_HELPER(index)                                       \
    zmx = z - x;                                               \
    zpx = z + x;                                               \
    (*cascaded_shadow_frustum_partitions)[index][0] = zmx - y; \
    (*cascaded_shadow_frustum_partitions)[index][1] = zpx - y; \
    (*cascaded_shadow_frustum_partitions)[index][2] = zpx + y; \
    (*cascaded_shadow_frustum_partitions)[index][3] = zmx + y

    GX_HELPER(0);

    x = xtanx * -uniform->far;
    y = ytany * -uniform->far;
    z = uniform->position + (uniform->z * uniform->far);

    GX_HELPER(sections_count);

    if (sections_count < 2) {
        // it provides a little performance gain for poor hardwares
        return;
    }

    // Zi = yn(f/n)^(i/N) + (1-y)(n+(i/N)(f-n))
    // Zi = yn((f/n)^(1/N))^i + (1-y)n + (1-y)((f-n)/N)i
    const core::Real oneminlambda = 1.0f - lambda;
    const core::Real onedivcn = 1.0f / static_cast<core::Real>(sections_count);
    // uniform increament
    const core::Real unisecinc = oneminlambda * onedivcn * (uniform->far - uniform->near);
    const core::Real fdivn = uniform->far / uniform->near;
    // logarithmic multiplication
    const core::Real logsecmul = std::pow(fdivn, onedivcn);
    // uniform sector
    core::Real unisec = oneminlambda * uniform->near;
    // logarithmic sector
    core::Real logsec = lambda * uniform->near;

    for (std::size_t i = 1; i < sections_count; ++i) {
        logsec *= logsecmul;
        unisec += unisecinc;

        const core::Real l = logsec + unisec;
        x = xtanx * l;
        y = ytany * l;
        z = uniform->position + (uniform->z * l);

        GX_HELPER(i);
    }

#undef GX_HELPER
}

gearoenix::render::camera::Perspective::Perspective(
    const core::Id my_id,
    system::stream::Stream*const f,
    engine::Engine*const e) noexcept
    : Camera(my_id, f, e)
{
    transformation->set_on_frustum_update([this] { update_cascades(); });
    const auto rad = f->read<core::Real>();
    GXLOGD("Radiant is: " << rad << ", in perspective camera with id: " << my_id)
    set_field_of_view(rad);
}

gearoenix::render::camera::Perspective::Perspective(
    const core::Id my_id,
    engine::Engine*const e) noexcept
    : Camera(my_id, e)
{
    transformation->set_on_frustum_update([this] { update_cascades(); });
    set_field_of_view(1.571f);
}

void gearoenix::render::camera::Perspective::set_aspect_ratio(const gearoenix::core::Real ratio) noexcept
{
    Camera::set_aspect_ratio(ratio);
    tany = tanx / ratio;
    update_fovy();
}

void gearoenix::render::camera::Perspective::set_field_of_view(const core::Real radian) noexcept
{
    fovx = radian;
    tanx = static_cast<core::Real>(std::tan(static_cast<double>(radian) * 0.5));
    tany = tanx / uniform->aspect_ratio;
    update_fovy();
}

gearoenix::math::Ray3 gearoenix::render::camera::Perspective::create_ray3(const core::Real x, const core::Real y) const noexcept
{
    math::Vec3 dir = (uniform->x * x) + (uniform->y * y) + (uniform->z * uniform->near);
    const math::Vec3 origin = dir + uniform->position;
    dir.normalize();
    return math::Ray3(origin, dir);
}

gearoenix::core::Real gearoenix::render::camera::Perspective::get_distance(const math::Vec3& model_location) const noexcept
{
    return (model_location - uniform->position).square_length();
}