#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"

namespace {
gearoenix::core::allocator::SharedArray<gearoenix::gl::Camera, gearoenix::render::camera::Camera::MAX_COUNT> allocator;
}

const boost::container::flat_set<std::type_index>& gearoenix::gl::Camera::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { create_type_index<Camera>(), create_this_type_index(this) };
    return types;
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_customised_target = std::dynamic_pointer_cast<Target>(t);
    render::camera::Camera::set_customised_target(std::move(t));
}

gearoenix::gl::Camera::Camera(
    Engine& e,
    const std::string& name,
    const std::size_t resolution_cfg_listener,
    std::shared_ptr<Target>&& customised_target,
    const render::camera::ProjectionData projection_data,
    const float near,
    const float far,
    const bool has_bloom)
    : render::camera::Camera(
          e, std::type_index(typeid(Camera)), name, resolution_cfg_listener,
          std::move(customised_target), projection_data, near, far, has_bloom)
{
}

std::shared_ptr<gearoenix::gl::Camera> gearoenix::gl::Camera::construct(
    Engine& e,
    const std::string& name,
    const std::size_t resolution_cfg_listener,
    std::shared_ptr<Target>&& customised_target,
    render::camera::ProjectionData projection_data,
    const float near,
    const float far,
    const bool has_bloom)
{
    auto self = allocator.make_shared(e, name, resolution_cfg_listener, std::move(customised_target), projection_data, near, far, has_bloom);
    self->set_component_self(self);
    return self;
}

gearoenix::gl::Camera::~Camera() = default;

gearoenix::gl::CameraBuilder::CameraBuilder(Engine& e, const std::string& name, core::job::EndCaller<>&& end_caller)
    : Builder(e, name, std::move(end_caller))
    , eng(e)
{
    auto& builder = entity_builder->get_builder();
    auto camera = Camera::construct(e, name + "-gl-camera", 0);
    builder.add_component(std::move(camera));
}

gearoenix::gl::CameraBuilder::~CameraBuilder() = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::gl::CameraManager::build_v(
    const std::string& name, core::job::EndCaller<>&& end_caller)
{
    return std::make_shared<CameraBuilder>(dynamic_cast<Engine&>(e), name, std::move(end_caller));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e)
    : Manager(e)
{
}

void gearoenix::gl::CameraManager::window_resized()
{
    Manager::window_resized();
}

gearoenix::gl::CameraManager::~CameraManager() = default;

#endif
