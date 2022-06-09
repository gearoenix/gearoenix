#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"

gearoenix::gl::Camera::Camera() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::gl::Camera::~Camera() noexcept = default;

gearoenix::gl::Camera::Camera(Camera&&) noexcept = default;

gearoenix::gl::CameraBuilder::CameraBuilder(Engine& e, const std::string& name) noexcept
    : render::camera::Builder(e, name)
    , eng(e)
{
    auto& builder = entity_builder->get_builder();
    builder.add_component(Camera());
}

void gearoenix::gl::CameraBuilder::set_target(std::shared_ptr<render::texture::Target>&& target) noexcept
{
    entity_builder->get_builder().get_component<Camera>()->target = std::dynamic_pointer_cast<Target>(target);
    render::camera::Builder::set_target(std::move(target));
}

gearoenix::gl::CameraBuilder::~CameraBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::gl::CameraManager::build(const std::string& name) noexcept
{
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
}

gearoenix::gl::CameraManager::~CameraManager() noexcept = default;

#endif
