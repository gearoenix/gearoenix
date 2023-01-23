#ifndef GEAROENIX_GL_CAMERA_HPP
#define GEAROENIX_GL_CAMERA_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/camera/gx-rnd-cmr-builder.hpp"
#include "../render/camera/gx-rnd-cmr-manager.hpp"
#include <array>
#include <optional>

namespace gearoenix::render::camera {
struct BloomData;
}

namespace gearoenix::gl {
struct Engine;
struct Target;
struct Texture2D;

struct BloomData final {
    typedef std::array<std::shared_ptr<Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> Targets;
    typedef std::array<std::shared_ptr<Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - 1> TargetsLess;

    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, horizontal_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, vertical_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Target>, source_target);
    GX_GET_CREF_PRV(std::shared_ptr<Target>, prefilter_target);
    GX_GET_CREF_PRV(Targets, horizontal_targets);
    GX_GET_CREF_PRV(TargetsLess, vertical_targets);
    GX_GET_CREF_PRV(Targets, upsampler_targets);

    BloomData(
        std::shared_ptr<Texture2D>&& horizontal_texture,
        std::shared_ptr<Texture2D>&& vertical_texture,
        std::shared_ptr<Target>&& source_target,
        std::shared_ptr<Target>&& prefilter_target,
        Targets&& horizontal_targets,
        TargetsLess&& vertical_targets,
        Targets&& upsampler_targets) noexcept;

public:
    ~BloomData() noexcept;
    [[nodiscard]] static std::optional<BloomData> construct(const std::optional<render::camera::BloomData>& bloom_data) noexcept;
};

struct Camera final : public core::ecs::Component {
    friend struct CameraBuilder;

    GX_GET_CREF_PRV(std::shared_ptr<Target>, target);
    GX_GET_CREF_PRV(std::optional<gl::BloomData>, bloom_data);

private:
    void disable_bloom() noexcept;

public:
    Camera(std::string&& name, const render::camera::Camera& camera) noexcept;
    ~Camera() noexcept final;
    Camera(Camera&&) noexcept;
};

struct CameraBuilder final : public render::camera::Builder {
    friend struct CameraManager;
    Engine& eng;

private:
    CameraBuilder(Engine& e, const std::string& name, core::sync::EndCaller&& end_caller) noexcept;
    void set_customised_target(std::shared_ptr<render::texture::Target>&& target) noexcept final;
    void disable_bloom() noexcept final;

public:
    ~CameraBuilder() noexcept final;
};

struct CameraManager final : public render::camera::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::camera::Builder> build(const std::string& name, core::sync::EndCaller&& end_caller) noexcept final;

public:
    explicit CameraManager(Engine& e) noexcept;
    ~CameraManager() noexcept final;
};
}

#endif
#endif
