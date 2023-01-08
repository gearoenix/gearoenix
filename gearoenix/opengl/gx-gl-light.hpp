#ifndef GEAROENIX_GL_LIGHT_HPP
#define GEAROENIX_GL_LIGHT_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/light/gx-rnd-lt-builder.hpp"
#include "../render/light/gx-rnd-lt-manager.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Engine;
struct Texture2D;
struct Target;

struct ShadowCasterDirectionalLight final : public core::ecs::Component {
    friend struct LightBuilder;
    friend struct LightManager;

    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, shadow_map_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Target>, shadow_map_target);
    GX_GET_VAL_PRV(uint, shadow_map_texture_v, static_cast<uint>(-1));
    GX_GET_VAL_PRV(uint, shadow_map_target_v, static_cast<uint>(-1));

public:
    explicit ShadowCasterDirectionalLight(std::string&& name) noexcept;
    ~ShadowCasterDirectionalLight() noexcept final;
    ShadowCasterDirectionalLight(ShadowCasterDirectionalLight&&) noexcept;
};

struct LightBuilder final : public render::light::Builder {
    friend struct LightManager;
    Engine& eng;

private:
    LightBuilder(
        Engine& e,
        const std::string& name,
        const render::light::Builder::DirectionalInfo& info,
        const core::sync::EndCaller& end_callback) noexcept;

    LightBuilder(
        Engine& e,
        const std::string& name,
        const render::light::Builder::ShadowCasterDirectionalInfo& info,
        const core::sync::EndCaller& end_callback) noexcept;

public:
    ~LightBuilder() noexcept final;
};

struct LightManager final : public render::light::Manager {
    Engine& eng;

private:
    [[nodiscard]] std::shared_ptr<render::light::Builder> build_directional(
        const std::string& name,
        const core::sync::EndCaller& end_callback) noexcept final;
    [[nodiscard]] std::shared_ptr<render::light::Builder> build_shadow_caster_directional(
        const std::string& name,
        std::size_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        const core::sync::EndCaller& end_callback) noexcept final;

public:
    explicit LightManager(Engine& e) noexcept;
    ~LightManager() noexcept final;
};
}

#endif
#endif
