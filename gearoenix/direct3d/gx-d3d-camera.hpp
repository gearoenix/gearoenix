#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/camera/gx-rnd-cmr-builder.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"
#include "gx-d3d-buffer.hpp"
#include "gx-d3d-build-configuration.hpp"
#include "gx-d3d-loader.hpp"
#include <array>

namespace gearoenix::d3d {
struct Camera final : public core::ecs::Component {
    std::array<UniformBuffer, GX_D3D_FRAMES_BACKBUFFER_NUMBER> uniforms;

    Camera(Engine& e, const std::string& name);
    ~Camera() final;
    Camera(Camera&&);
};

struct CameraBuilder final : public render::camera::Builder {
    friend struct CameraManager;

private:
    CameraBuilder(Engine& e, const std::string& name);

public:
    ~CameraBuilder() final;
};

struct CameraManager final : public render::camera::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::camera::Builder> build(const std::string& name) final;

public:
    explicit CameraManager(Engine& e);
    ~CameraManager() final;
};
}

#endif