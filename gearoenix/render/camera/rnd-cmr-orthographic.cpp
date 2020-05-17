#include "rnd-cmr-orthographic.hpp"
#include "../../physics/collider/phs-cld-frustum.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-transformation.hpp"
#include "rnd-cmr-uniform.hpp"
#include <cmath>

void gearoenix::render::camera::Orthographic::update_aspects_size() noexcept
{
    uniform.projection = math::Mat4x4<float>::orthographic(
        static_cast<float>(aspects_size * uniform.aspect_ratio * 2.0),
        static_cast<float>(aspects_size * 2.0),
        std::abs(uniform.near),
        std::abs(uniform.far));
    uniform.uniform_projection = math::Mat4x4(
                                     0.5f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 0.5f, 0.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.5f, 0.5f, 0.0f, 1.0f)
        * uniform.projection;
    reinterpret_cast<Transformation*>(transformation.get())->update_view_projection();
}

void gearoenix::render::camera::Orthographic::update_cascades() noexcept
{
    const system::Configuration& sys_conf = render_engine->get_system_application()->get_configuration();
    const engine::Configuration& eng_conf = sys_conf.render_config;
    const std::size_t sections_count = eng_conf.get_shadow_cascades_count();
    const auto sections_count_plus = sections_count + 1;

    if (cascaded_shadow_frustum_partitions.size() != sections_count_plus)
        cascaded_shadow_frustum_partitions.resize(sections_count_plus);

    const math::Vec3 x = transformation->get_x_axis() * aspects_size * uniform.aspect_ratio;
    const math::Vec3 y = transformation->get_y_axis() * aspects_size;
    const math::Vec3 z = math::Vec3<double>(uniform.position) + (transformation->get_z_axis() * uniform.near);

    const math::Vec3 zmx = z - x;
    const math::Vec3 zpx = z + x;

    cascaded_shadow_frustum_partitions[0][0] = zmx - y;
    cascaded_shadow_frustum_partitions[0][1] = zpx - y;
    cascaded_shadow_frustum_partitions[0][2] = zpx + y;
    cascaded_shadow_frustum_partitions[0][3] = zmx + y;

    const math::Vec3 z_inc = transformation->get_z_axis() * ((uniform.far - uniform.near) / double(sections_count));

    for (std::size_t i = 1, j = 0; i < sections_count_plus; ++i, ++j)
        for (int k = 0; k < 4; ++k)
            cascaded_shadow_frustum_partitions[i][k] = cascaded_shadow_frustum_partitions[j][k] + z_inc;

    const auto& far_points = cascaded_shadow_frustum_partitions[sections_count];
    const auto& near_points = cascaded_shadow_frustum_partitions[0];

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
}

gearoenix::render::camera::Orthographic::Orthographic(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : Camera(my_id, f, e)
{
    reinterpret_cast<Transformation*>(transformation.get())->set_on_frustum_update([this] { update_cascades(); });
    aspects_size = static_cast<double>(f->read<float>()) / 4.0;
    GXLOGD("Aspect size is: " << aspects_size << ", in orthographic camera with id: " << my_id)
    update_aspects_size();
}

gearoenix::render::camera::Orthographic::Orthographic(core::Id my_id, engine::Engine* e) noexcept
    : Camera(my_id, e)
    , aspects_size(1.0f)
{
    reinterpret_cast<Transformation*>(transformation.get())->set_on_frustum_update([this] { update_cascades(); });
    update_aspects_size();
}

gearoenix::math::Ray3 gearoenix::render::camera::Orthographic::create_ray3(const double x, const double y) const noexcept
{
    const math::Vec3 dir = -transformation->get_z_axis();
    const math::Vec3 origin = (transformation->get_x_axis() * x) + (transformation->get_y_axis() * y) + (dir * -uniform.near) + math::Vec3<double>(uniform.position);
    return math::Ray3(origin, dir);
}

double gearoenix::render::camera::Orthographic::get_distance(const math::Vec3<double>& model_location) const noexcept
{
    return (math::Vec3<double>(uniform.position) - model_location).dot(transformation->get_z_axis());
}

void gearoenix::render::camera::Orthographic::set_aspects_size(const double a) noexcept
{
    aspects_size = a;
    update_aspects_size();
}

void gearoenix::render::camera::Orthographic::set_aspects(const unsigned int w, unsigned int h) noexcept
{
    Camera::set_aspects(w, h);
    update_aspects_size();
}
