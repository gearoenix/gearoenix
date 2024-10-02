#ifndef GEAROENIX_GL_LIGHT_HPP
#define GEAROENIX_GL_LIGHT_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/light/gx-rnd-lt-builder.hpp"
#include "../render/light/gx-rnd-lt-directional.hpp"
#include "../render/light/gx-rnd-lt-manager.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Engine;
struct Texture2D;
struct Target;

struct ShadowCasterDirectionalLight final : render::light::ShadowCasterDirectional {
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, shadow_map_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Target>, shadow_map_target);
    GX_GET_VAL_PRV(uint, shadow_map_texture_v, static_cast<uint>(-1));
    GX_GET_VAL_PRV(uint, shadow_map_target_v, static_cast<uint>(-1));

    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;

public:
    explicit ShadowCasterDirectionalLight(std::string&& name, core::ecs::entity_id_t entity_id);
    [[nodiscard]] static std::shared_ptr<ShadowCasterDirectionalLight> construct(std::string&& name, core::ecs::entity_id_t entity_id);
    ~ShadowCasterDirectionalLight() override;
    void set_shadow_map(std::shared_ptr<render::texture::Texture2D>&& t, core::job::EndCaller<>&& end_callback) override;
    void set_shadow_map_target(std::shared_ptr<render::texture::Target>&& t) override;
};

struct LightBuilder final : render::light::Builder {
    Engine& eng;

private:
    LightBuilder(
        Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        const ShadowCasterDirectionalInfo& info,
        core::job::EndCaller<>&& end_callback);

public:
    LightBuilder(
        Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        const DirectionalInfo& info,
        core::job::EndCaller<>&& end_callback);
    static void construct(
        Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        const ShadowCasterDirectionalInfo& info,
        core::job::EndCallerShared<Builder>&& end_callback,
        core::job::EndCaller<>&& entity_end_callback);
    ~LightBuilder() override;
};

struct LightManager final : render::light::Manager {
    Engine& eng;

private:
    [[nodiscard]] std::shared_ptr<render::light::Builder> build_directional(
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        core::job::EndCaller<>&& end_callback) override;
    void build_shadow_caster_directional(
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        std::uint32_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        core::job::EndCallerShared<render::light::Builder>&& end_callback,
        core::job::EndCaller<>&& entity_end_callback) override;

public:
    explicit LightManager(Engine& e);
    ~LightManager() override;
};
}

#endif
#endif
