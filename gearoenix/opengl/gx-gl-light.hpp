#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/light/gx-rnd-lt-directional.hpp"
#include "../render/light/gx-rnd-lt-manager.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Texture2D;
struct Target;

struct ShadowCasterDirectionalLight final : render::light::ShadowCasterDirectional {
    constexpr static auto object_type_index = gearoenix_gl_light_shadow_caster_directional_type_index;
    constexpr static std::array all_parent_object_type_indices { ShadowCasterDirectional::object_type_index, Light::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { ShadowCasterDirectional::object_type_index };

    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, shadow_map_texture);
    GX_GET_CREF_PRV(std::shared_ptr<Target>, shadow_map_target);
    GX_GET_VAL_PRV(uint, shadow_map_texture_v, static_cast<uint>(-1));
    GX_GET_VAL_PRV(uint, shadow_map_target_v, static_cast<uint>(-1));

    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&& stream, core::job::EndCaller<>&& end_callback) const;

public:
    ShadowCasterDirectionalLight(core::ecs::Entity* entity, std::string&& name);
    ~ShadowCasterDirectionalLight() override;
    void set_shadow_map(std::shared_ptr<render::texture::Texture2D>&& t, core::job::EndCaller<>&& end_callback) override;
    void set_shadow_map_target(std::shared_ptr<render::texture::Target>&& t) override;
};

struct LightManager final : render::light::Manager {
private:
    [[nodiscard]] core::ecs::EntityPtr build_directional(std::string&&, core::ecs::Entity*) override;
    void build_shadow_caster_directional(
        std::string&& name, core::ecs::Entity* parent, std::uint32_t shadow_map_resolution,
        float camera_far, float camera_near, float camera_aspect,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;

public:
    LightManager();
    ~LightManager() override;
};
}

#endif