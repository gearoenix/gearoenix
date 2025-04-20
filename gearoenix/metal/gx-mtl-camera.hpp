#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-component.hpp"
#import "../render/camera/gx-rnd-cmr-builder.hpp"
#import "../render/camera/gx-rnd-cmr-manager.hpp"
#import "gx-mtl-buffer.hpp"

namespace gearoenix::metal {
struct Engine;
struct Camera final : public core::ecs::Component {
    UniformBuffer uniform;

    Camera(Engine& e, const std::string& name);
    ~Camera() override;
    Camera(Camera&&);
};

struct CameraBuilder final : public render::camera::Builder {
    friend struct CameraManager;

private:
    CameraBuilder(Engine& e, const std::string& name);

public:
    ~CameraBuilder() override;
};

struct CameraManager final : public render::camera::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::camera::Builder> build(const std::string& name) override;

public:
    explicit CameraManager(Engine& e);
    ~CameraManager() override;
};
}

#endif