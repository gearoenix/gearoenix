#include "gx-d3d-camera.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-string.hpp"
#include "gx-d3d-buffer.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-engine.hpp"
#include "shaders/gx-d3d-shd-common.hpp"
#include <map>

gearoenix::d3d::Camera::Camera(Engine& e, const std::string& name)
    : core::ecs::Component(this)
    , uniforms {
        UniformBuffer(e, sizeof(CameraUniform), core::String::to_wchar_ptr(name + "-D3D-Uniform-0")),
        UniformBuffer(e, sizeof(CameraUniform), core::String::to_wchar_ptr(name + "-D3D-Uniform-1")),
        UniformBuffer(e, sizeof(CameraUniform), core::String::to_wchar_ptr(name + "-D3D-Uniform-2")),
    }
{
}

gearoenix::d3d::Camera::~Camera() = default;

gearoenix::d3d::Camera::Camera(Camera&&) = default;

gearoenix::d3d::CameraBuilder::CameraBuilder(Engine& e, const std::string& name)
    : render::camera::Builder(e, name)
{
    entity_builder->get_builder().add_component(Camera(e, name));
}

gearoenix::d3d::CameraBuilder::~CameraBuilder() = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::d3d::CameraManager::build(const std::string& name) { return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name)); }

gearoenix::d3d::CameraManager::CameraManager(Engine& e)
    : render::camera::Manager(e)
{
}

gearoenix::d3d::CameraManager::~CameraManager() = default;

#endif
