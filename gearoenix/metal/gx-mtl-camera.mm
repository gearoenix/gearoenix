#include "gx-mtl-camera.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "shaders/gx-mtl-shd-common.hpp"
#import "../core/ecs/gx-cr-ecs-entity.hpp"

gearoenix::metal::Camera::Camera(Engine& e, const std::string& name) noexcept : core::ecs::Component(this) {
    for(std::size_t uniform_index = 0; uniform_index < uniforms.size(); ++uniform_index) {
        auto& uniform = uniforms[uniform_index];
        uniform = [e.get_device() newBufferWithLength:sizeof(CameraUniform) options:MTLResourceStorageModeShared];
        uniform.label = [NSString stringWithFormat:@"Gearoenix-CameraUniform-%s-%lu", name.c_str(), uniform_index];
    }
}

gearoenix::metal::Camera::~Camera() noexcept {
    for(auto& uniform: uniforms) if(nil != uniform) [uniform release];
}

gearoenix::metal::Camera::Camera(Camera&& o) noexcept : core::ecs::Component(this) {
    for(std::size_t uniform_index = 0; uniform_index < uniforms.size(); ++uniform_index) {
        auto& uniform = o.uniforms[uniform_index];
        uniforms[uniform_index] = uniform;
        uniform = nil;
    }
}

gearoenix::metal::CameraBuilder::CameraBuilder(Engine& e, const std::string& name) noexcept : render::camera::Builder(e, name) {
    entity_builder->get_builder().add_component(Camera(e, name));
}

gearoenix::metal::CameraBuilder::~CameraBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::metal::CameraManager::build(const std::string& name) noexcept {
    return std::shared_ptr<render::camera::Builder>(new CameraBuilder(dynamic_cast<Engine&>(e), name));
}

gearoenix::metal::CameraManager::CameraManager(Engine& e) noexcept
    : render::camera::Manager(e)
{
}

gearoenix::metal::CameraManager::~CameraManager() noexcept = default;

#endif
