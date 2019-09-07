#include "rnd-cmr-orthographic.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../system/sys-configuration.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-transformation.hpp"
#include "rnd-cmr-uniform.hpp"
#include <cmath>

void gearoenix::render::camera::Orthographic::update_aspects_size() noexcept
{
	uniform->projection = math::Mat4x4::orthographic(
		aspects_size * uniform->aspect_ratio * 2.0f,
		aspects_size,
		std::abs(uniform->near),
		std::abs(uniform->far));
	uniform->uniform_projection = math::Mat4x4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f)
		* uniform->projection;
	transformation->update_view_projection();
}

void gearoenix::render::camera::Orthographic::update_cascades() noexcept
{
	const system::Configuration& sys_conf = e->get_system_application()->get_configuration();
	const engine::Configuration& eng_conf = sys_conf.render_config;
	const std::size_t sections_count = eng_conf.shadow_cascades_count;
	const auto sections_count_plus = sections_count + 1;

	if (cascaded_shadow_frustum_partitions->size() != sections_count_plus)
		cascaded_shadow_frustum_partitions->resize(sections_count_plus);

	const math::Vec3 x = uniform->x * aspects_size * uniform->aspect_ratio;
	const math::Vec3 y = uniform->y * aspects_size;
	const math::Vec3 z = uniform->position + (uniform->z * uniform->near);

	const math::Vec3 zmx = z - x;
	const math::Vec3 zpx = z + x;

    (*cascaded_shadow_frustum_partitions)[0][0] = zmx - y;
    (*cascaded_shadow_frustum_partitions)[0][1] = zpx - y;
    (*cascaded_shadow_frustum_partitions)[0][2] = zpx + y;
	(*cascaded_shadow_frustum_partitions)[0][3] = zmx + y;

	const math::Vec3 z_inc = uniform->z * (std::abs(uniform->far) - std::abs(uniform->near));

	for (std::size_t i = 1, j = 0; i < sections_count_plus; ++i, ++j)
		for(int k = 0; k < 4; ++k)
			(*cascaded_shadow_frustum_partitions)[i][k] = (*cascaded_shadow_frustum_partitions)[j][k] + z_inc;
}

gearoenix::render::camera::Orthographic::Orthographic(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : Camera(my_id, f, e)
{
	transformation->set_on_frustum_update([this] { update_cascades(); });
	f->read(aspects_size);
	GXLOGD("Aspect size is: " << aspects_size << ", in orthographic camera with id: " << my_id)
	update_aspects_size();
}

gearoenix::render::camera::Orthographic::Orthographic(core::Id my_id, engine::Engine* e) noexcept
    : Camera(my_id, e)
    , aspects_size(1.0f)
{
    transformation->set_on_frustum_update([this] { update_cascades(); });
    update_aspects_size();
}

gearoenix::math::Ray3 gearoenix::render::camera::Orthographic::create_ray3(const core::Real x, const core::Real y) const noexcept
{
    const math::Vec3 dir = -uniform->z;
    const math::Vec3 origin = (uniform->x * x) + (uniform->y * y) + (dir * -uniform->near) + uniform->position;
    return math::Ray3(origin, dir);
}

gearoenix::core::Real gearoenix::render::camera::Orthographic::get_distance(const math::Vec3& model_location) const noexcept
{
    return (uniform->position - model_location).dot(uniform->z);
}

void gearoenix::render::camera::Orthographic::set_aspects_size(const core::Real a) noexcept
{
	aspects_size = a;
	update_aspects_size();
}

void gearoenix::render::camera::Orthographic::set_aspect_ratio(const gearoenix::core::Real ratio) noexcept
{
	Camera::set_aspect_ratio(ratio);
	update_aspects_size();
}
