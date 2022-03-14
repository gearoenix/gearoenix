#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"

gearoenix::gl::CameraBuilder::CameraBuilder(Engine& e, const std::string& name) noexcept
    : render::camera::Builder(e, name)
{
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
