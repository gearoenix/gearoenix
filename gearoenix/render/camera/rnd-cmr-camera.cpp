#include "rnd-cmr-camera.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../math/math-projector-frustum.hpp"
#include "../../math/math-quaternion.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-configuration.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-transformation.hpp"
#include "rnd-cmr-uniform.hpp"

gearoenix::render::camera::Camera::Camera(const core::Id my_id, engine::Engine* const e) noexcept
    : core::asset::Asset(my_id, core::asset::Type::CAMERA)
    , e(e)
    , uniform(new Uniform)
    , frustum(new math::ProjectorFrustum(math::Mat4x4()))
    , uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
    , cascaded_shadow_frustum_partitions(new std::vector<std::array<math::Vec3, 4>>(
          e->get_system_application()->get_configuration().render_config.shadow_cascades_count + 1))
{
    transformation = std::make_shared<Transformation>(uniform, frustum, cascaded_shadow_frustum_partitions);
    uniform->aspect_ratio = e->get_system_application()->get_window_ratio();
}

gearoenix::render::camera::Camera::Camera(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e) noexcept
    : core::asset::Asset(my_id, core::asset::Type::CAMERA)
    , e(e)
    , uniform(new Uniform)
    , frustum(new math::ProjectorFrustum(math::Mat4x4()))
    , uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
    , cascaded_shadow_frustum_partitions(new std::vector<std::array<math::Vec3, 4>>(
          e->get_system_application()->get_configuration().render_config.shadow_cascades_count + 1))
{
    transformation = std::make_shared<Transformation>(uniform, frustum, cascaded_shadow_frustum_partitions);
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
    GXLOGD("Position: " << uniform->position)
    GXLOGD("Quaternion: " << q)
    GXLOGD("Near: " << uniform->near)
    transformation->update_location();
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::camera::Camera::get_uniform_buffers() const
{
    return uniform_buffers;
}

const std::shared_ptr<gearoenix::physics::Transformation> gearoenix::render::camera::Camera::get_transformation() const noexcept
{
    return transformation;
}

bool gearoenix::render::camera::Camera::is_enabled() const noexcept
{
    return enabled;
}

void gearoenix::render::camera::Camera::enable() noexcept
{
    enabled = true;
}

void gearoenix::render::camera::Camera::disable() noexcept
{
    enabled = false;
}

bool gearoenix::render::camera::Camera::in_sight(const gearoenix::math::Vec3& location, const core::Real radius) const noexcept
{
    return frustum->check_intersection(location, radius);
}

void gearoenix::render::camera::Camera::update_uniform()
{
    uniform_buffers->update(uniform.get());
}

void gearoenix::render::camera::Camera::set_aspect_ratio(const gearoenix::core::Real ratio)
{
    uniform->aspect_ratio = ratio;
}

const std::vector<std::array<gearoenix::math::Vec3, 4>>&
gearoenix::render::camera::Camera::get_cascaded_shadow_frustum_partitions() noexcept
{
    return *cascaded_shadow_frustum_partitions;
}
