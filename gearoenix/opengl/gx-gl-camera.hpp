#ifndef GEAROENIX_GL_CAMERA_HPP
#define GEAROENIX_GL_CAMERA_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/camera/gx-rnd-cmr-builder.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"

namespace gearoenix::gl {
struct Engine;
struct Target;

struct Camera final : public core::ecs::Component {
    std::shared_ptr<Target> target;

    Camera() noexcept;
    ~Camera() noexcept final;
    Camera(Camera&&) noexcept;
};

struct CameraBuilder final : public render::camera::Builder {
    friend struct CameraManager;

private:
    CameraBuilder(Engine& e, const std::string& name) noexcept;
    void set_target(std::shared_ptr<render::texture::Target>&& target) noexcept final;

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
