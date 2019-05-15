#include "rnd-cmr-camera.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../math/math-quaternion.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-uniform.hpp"
#include "rnd-cmr-transformation.hpp"

gearoenix::render::camera::Camera::Camera(
	const core::Id my_id, const std::shared_ptr<engine::Engine>& e) noexcept
	: core::asset::Asset(my_id, core::asset::Type::CAMERA)
	, uniform(new Uniform)
	, uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
{
	transformation = std::make_shared<Transformation>(uniform);
	uniform->aspect_ratio = e->get_system_application()->get_window_ratio();
}

gearoenix::render::camera::Camera::Camera(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e)
    : core::asset::Asset(my_id, core::asset::Type::CAMERA)
	, uniform(new Uniform)
    , uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
{
	transformation = std::make_shared<Transformation>(uniform);
	uniform->aspect_ratio = e->get_system_application()->get_window_ratio();
    uniform->position.read(f);
    math::Quat q;
    f->read(q.w);
    f->read(q.x);
    f->read(q.y);
    f->read(q.z);
    uniform->near = -f->read<core::Real>();
    uniform->far = -f->read<core::Real>();
    const math::Mat4x4 r = q.to_mat();
    uniform->x = r * uniform->x;
    uniform->y = r * uniform->y;
    uniform->z = r * uniform->z;
    uniform->inversed_rotation = math::Quat(q.x, q.y, q.z, -q.w).to_mat();
    GXLOGD("Position: " << uniform->position);
    GXLOGD("Quaternion: " << q);
    GXLOGD("Near: " << uniform->near);
    transformation->update_location();
}

gearoenix::render::camera::Camera::~Camera()
{
}

void gearoenix::render::camera::Camera::update_uniform()
{
	uniform_buffers->update(&uniform);
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::camera::Camera::get_uniform_buffers() const
{
    return uniform_buffers;
}

const std::shared_ptr<gearoenix::physics::Transformation>& gearoenix::render::camera::Camera::get_transformation() const noexcept
{
	return transformation;
}