#ifndef GEAROENIX_METAL_CAMERA_HPP
#define GEAROENIX_METAL_CAMERA_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-component.hpp"
#import "../render/camera/gx-rnd-cmr-builder.hpp"
#import "../render/camera/gx-rnd-cmr-manager.hpp"
#import "shaders/gx-mtl-shd-common.hpp"
#import <Metal/MTLBuffer.h>
#import <array>
#import <memory>
#import <string>

namespace gearoenix::metal {
struct Engine;
struct Camera final : public core::ecs::Component {
    std::array<id<MTLBuffer>, GEAROENIX_METAL_FRAMES_COUNT> uniforms;

    Camera(Engine& e, const std::string& name) noexcept;
    ~Camera() noexcept final;
    Camera(Camera&&) noexcept;
};

struct CameraBuilder final : public render::camera::Builder {
    friend struct CameraManager;

private:
    CameraBuilder(Engine& e, const std::string& name) noexcept;

public:
    ~CameraBuilder() noexcept final;
};

struct CameraManager final : public render::camera::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::camera::Builder> build(const std::string& name) noexcept final;

public:
    explicit CameraManager(Engine& e) noexcept;
    ~CameraManager() noexcept final;
};
}

#endif
#endif
