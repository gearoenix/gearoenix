#include "gx-mtl-camera.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-entity.hpp"
#import "gx-mtl-engine.hpp"
#import "shaders/gx-mtl-shd-common.hpp"

gearoenix::metal::Camera::Camera(Engine& e, const std::string& name) noexcept
    : core::ecs::Component(this)
    , uniform(e.get_buffer_manager()->create_uniform(sizeof(CameraUniform)))
{
}

gearoenix::metal::Camera::~Camera() noexcept = default;

gearoenix::metal::Camera::Camera(Camera&& o) noexcept = default;

gearoenix::metal::CameraBuilder::CameraBuilder(Engine& e, const std::string& name) noexcept
    : render::camera::Builder(e, name)
{
    entity_builder->get_builder().add_component(Camera(e, name));
}

gearoenix::metal::CameraBuilder::~CameraBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::metal::CameraManager::build(const std::string& name) noexcept
{
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name));
}

gearoenix::metal::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
}

gearoenix::metal::CameraManager::~CameraManager() noexcept = default;

#endif
