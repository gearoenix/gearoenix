#include "rnd-cmr-orthographic.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-cmr-uniform.hpp"
#include <cmath>

void gearoenix::render::camera::Orthographic::on_ratio_change() noexcept
{
    //    c_width = aspects_size * screen_ratio;
    //    p = math::Mat4x4::orthographic(c_width * 2.0f, c_height * 2.0f, start, end);
    //    vp = p * v;
}

gearoenix::render::camera::Orthographic::Orthographic(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : Camera(my_id, f, e)
{
    //    f->read(aspects_size);
    //    c_height = aspects_size;
    //    on_ratio_change();
}

gearoenix::math::Ray3 gearoenix::render::camera::Orthographic::create_ray3(const core::Real x, const core::Real y) const noexcept
{
    const math::Vec3 dir = -uniform->z;
    const math::Vec3 origin = (uniform->x * x) + (uniform->y * y) + (dir * uniform->near) + uniform->position;
    return math::Ray3(origin, dir);
}

gearoenix::core::Real gearoenix::render::camera::Orthographic::get_distance(const math::Vec3& model_location) const noexcept
{
    return (uniform->position - model_location).dot(uniform->z);
}
