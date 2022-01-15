#include "gx-dxr-camera.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-string.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "shaders/gx-dxr-shd-common.hpp"
#include <map>

gearoenix::dxr::Camera::Camera(Engine& e, const std::string& name) noexcept
    : core::ecs::Component(this)
    , uniforms {
        UniformBuffer(e, sizeof(CameraUniform), core::String::to_wchar_ptr(name + "-DXR-Uniform-0")),
        UniformBuffer(e, sizeof(CameraUniform), core::String::to_wchar_ptr(name + "-DXR-Uniform-1")),
        UniformBuffer(e, sizeof(CameraUniform), core::String::to_wchar_ptr(name + "-DXR-Uniform-2")),
    }
{
}

gearoenix::dxr::Camera::~Camera() noexcept = default;

gearoenix::dxr::Camera::Camera(Camera&&) noexcept = default;

gearoenix::dxr::CameraBuilder::CameraBuilder(Engine& e, const std::string& name) noexcept
    : render::camera::Builder(e, name)
{
    entity_builder->get_builder().add_component(Camera(e, name));
}

gearoenix::dxr::CameraBuilder::~CameraBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::dxr::CameraManager::build(const std::string& name) noexcept
{
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name));
}

gearoenix::dxr::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
}

gearoenix::dxr::CameraManager::~CameraManager() noexcept = default;

#endif
