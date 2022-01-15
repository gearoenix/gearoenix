#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/camera/gx-rnd-cmr-builder.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-build-configuration.hpp"
#include "gx-dxr-loader.hpp"
#include <array>

namespace gearoenix::dxr {
struct Camera final : public core::ecs::Component {
    std::array<UniformBuffer, GX_DXR_FRAMES_BACKBUFFER_NUMBER> uniforms;

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