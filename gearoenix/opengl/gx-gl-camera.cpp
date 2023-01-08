#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-target.hpp"

gearoenix::gl::Camera::Camera(std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
{
}

gearoenix::gl::Camera::~Camera() noexcept = default;

gearoenix::gl::Camera::Camera(Camera&&) noexcept = default;

gearoenix::gl::CameraBuilder::CameraBuilder(Engine& e, const std::string& name, core::sync::EndCaller&& end_caller) noexcept
    : render::camera::Builder(e, name, std::move(end_caller))
    , eng(e)
{
    auto& builder = entity_builder->get_builder();
    Camera camera(name + "-gl-camera");
    camera.target = std::dynamic_pointer_cast<Target>(entity_builder->get_builder().get_component<render::camera::Camera>()->get_target());
    builder.add_component(std::move(camera));
}

void gearoenix::gl::CameraBuilder::set_customised_target(std::shared_ptr<render::texture::Target>&& target) noexcept
{
    entity_builder->get_builder().get_component<Camera>()->target = std::dynamic_pointer_cast<Target>(target);
    render::camera::Builder::set_customised_target(std::move(target));
}

gearoenix::gl::CameraBuilder::~CameraBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::gl::CameraManager::build(const std::string& name, core::sync::EndCaller&& end_caller) noexcept
{
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name, std::move(end_caller)));
}

gearoenix::gl::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
    core::ecs::Component::register_type<Camera>();
}

gearoenix::gl::CameraManager::~CameraManager() noexcept = default;

#endif
