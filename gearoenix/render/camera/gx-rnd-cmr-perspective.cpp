#include "gx-rnd-cmr-perspective.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-transformation.hpp"
#include <cmath>

void gearoenix::render::camera::Perspective::update_fovy() noexcept
{
    fovy = std::atan(tany) * 2.0;
    update_projection();
    lambda = (std::sin(fovx * 0.5) + std::sin(fovy * 0.5)) * 0.5;
    reinterpret_cast<Transformation*>(transformation.get())->update_view_projection();
}

void gearoenix::render::camera::Perspective::update_cascades() noexcept
{
    const std::size_t sections_count = cascaded_shadow_frustum_partitions_count.has_value() ? cascaded_shadow_frustum_partitions_count.value() : render_engine->get_system_application()->get_configuration().render_config.get_shadow_cascades_count();
    if (cascaded_shadow_frustum_partitions.size() != sections_count + 1)
        cascaded_shadow_frustum_partitions.resize(sections_count + 1);

    const math::Vec3<double> xtanx = transformation->get_x_axis() * tanx;
    const math::Vec3<double> ytany = transformation->get_y_axis() * tany;

    math::Vec3<double> x = xtanx * -uniform.near;
    math::Vec3<double> y = ytany * -uniform.near;
    math::Vec3<double> z = math::Vec3<double>(uniform.position) + (transformation->get_z_axis() * static_cast<double>(uniform.near));

    math::Vec3<double> zmx;
    math::Vec3<double> zpx;

#define GX_HELPER(index)                                    \
    zmx = z - x;                                            \
    zpx = z + x;                                            \
    cascaded_shadow_frustum_partitions[index][0] = zmx - y; \
    cascaded_shadow_frustum_partitions[index][1] = zpx - y; \
    cascaded_shadow_frustum_partitions[index][2] = zpx + y; \
    cascaded_shadow_frustum_partitions[index][3] = zmx + y

    GX_HELPER(0);

    x = xtanx * -uniform.far;
    y = ytany * -uniform.far;
    z = math::Vec3<double>(uniform.position) + (transformation->get_z_axis() * static_cast<double>(uniform.far));

    GX_HELPER(sections_count);

    const auto& near_points = cascaded_shadow_frustum_partitions[0];
    const auto& far_points = cascaded_shadow_frustum_partitions[sections_count];

    frustum_collider->update({
        far_points[3],
        far_points[2],
        far_points[0],
        far_points[1],
        near_points[3],
        near_points[2],
        near_points[0],
        near_points[1],
    });

    if (sections_count < 2) {
        // it provides a little performance gain for poor hardwares
        return;
    }

    // Zi = yn(format/n)^(i/N) + (1-y)(n+(i/N)(format-n))
    // Zi = yn((format/n)^(1/N))^i + (1-y)n + (1-y)((format-n)/N)i
    const double oneminlambda = 1.0f - lambda;
    const double onedivcn = 1.0f / static_cast<double>(sections_count);
    // uniform increament
    const double unisecinc = oneminlambda * onedivcn * (uniform.far - uniform.near);
    const double fdivn = uniform.far / uniform.near;
    // logarithmic multiplication
    const double logsecmul = std::pow(fdivn, onedivcn);
    // uniform sector
    double unisec = oneminlambda * uniform.near;
    // logarithmic sector
    double logsec = lambda * uniform.near;

    for (std::size_t i = 1; i < sections_count; ++i) {
        logsec *= logsecmul;
        unisec += unisecinc;

        const double l = logsec + unisec;
        x = xtanx * l;
        y = ytany * l;
        z = math::Vec3<double>(uniform.position) + (transformation->get_z_axis() * l);

        GX_HELPER(i);
    }
#undef GX_HELPER
}

void gearoenix::render::camera::Perspective::update_projection() noexcept
{
    uniform.projection = math::Mat4x4<float>::perspective(
        static_cast<float>(tanx * uniform.near * -2.0),
        static_cast<float>(tany * uniform.near * -2.0),
        -uniform.near,
        -uniform.far);
    uniform.uniform_projection = math::Mat4x4(
                                     0.5f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 0.5f, 0.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.5f, 0.5f, 0.0f, 1.0f)
        * uniform.projection;
}

gearoenix::render::camera::Perspective::Perspective(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : Camera(my_id, std::move(name), f, e)
{
    reinterpret_cast<Transformation*>(transformation.get())->set_on_frustum_update([this] { update_cascades(); });
    reinterpret_cast<Transformation*>(transformation.get())->set_on_projection_update([this] { update_projection(); });
    const auto rad = f->read<float>();
    GXLOGD("Radiant is: " << rad << ", in perspective camera with id: " << my_id)
    set_field_of_view(rad);
}

gearoenix::render::camera::Perspective::Perspective(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e) noexcept
    : Camera(my_id, std::move(name), e)
{
    reinterpret_cast<Transformation*>(transformation.get())->set_on_frustum_update([this] { update_cascades(); });
    reinterpret_cast<Transformation*>(transformation.get())->set_on_projection_update([this] { update_projection(); });
    set_field_of_view(1.570796328f);
}

void gearoenix::render::camera::Perspective::set_aspects(const unsigned int w, const unsigned int h) noexcept
{
    Camera::set_aspects(w, h);
    tany = tanx / uniform.aspect_ratio;
    update_fovy();
}

void gearoenix::render::camera::Perspective::set_field_of_view(const double radian) noexcept
{
    fovx = radian;
    tanx = std::tan(radian * 0.5);
    tany = tanx / uniform.aspect_ratio;
    update_fovy();
}

gearoenix::math::Ray3 gearoenix::render::camera::Perspective::create_ray3(const double x, const double y) const noexcept
{
    math::Vec3<double> dir = (transformation->get_x_axis() * (x / uniform.aspect_ratio)) + (transformation->get_y_axis() * (y / uniform.aspect_ratio)) + (transformation->get_z_axis() * uniform.near);
    const math::Vec3<double> origin = dir + math::Vec3<double>(uniform.position);
    dir.normalize();
    return math::Ray3(origin, dir);
}

double gearoenix::render::camera::Perspective::get_distance(const math::Vec3<double>& model_location) const noexcept
{
    return (model_location - math::Vec3<double>(uniform.position)).square_length();
}