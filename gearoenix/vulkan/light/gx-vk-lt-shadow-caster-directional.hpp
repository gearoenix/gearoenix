#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/light/gx-rnd-lt-directional.hpp"

namespace gearoenix::render::record {
struct ShadowCasterDirectionalLightData;
}

namespace gearoenix::vulkan::texture {
struct Texture2D;
struct Target;
}

namespace gearoenix::vulkan::light {
struct ShadowCasterDirectional final : render::light::ShadowCasterDirectional {
    constexpr static auto object_type_index = gearoenix_gapi_light_shadow_caster_directional_type_index;
    constexpr static std::array all_parent_object_type_indices { render::light::ShadowCasterDirectional::object_type_index, Light::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::light::ShadowCasterDirectional::object_type_index };

private:
    std::shared_ptr<texture::Texture2D> vk_shadow_map_texture;
    std::shared_ptr<texture::Target> vk_shadow_map_target;

public:
    [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get_vk_shadow_map_texture() const { return vk_shadow_map_texture; }
    [[nodiscard]] const std::shared_ptr<texture::Target>& get_vk_shadow_map_target() const { return vk_shadow_map_target; }

    ShadowCasterDirectional(core::ecs::Entity* entity, std::string&& name);
    ~ShadowCasterDirectional() override;
    void set_shadow_map(std::shared_ptr<render::texture::Texture2D>&& t, core::job::EndCaller<>&& end_callback) override;
    void set_shadow_map_target(std::shared_ptr<render::texture::Target>&& t) override;
};
}
#endif