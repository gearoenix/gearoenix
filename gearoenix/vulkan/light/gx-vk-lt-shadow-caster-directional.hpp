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

    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, vk_shadow_map_texture);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, vk_shadow_map_target);
    GX_GET_VAL_PRV(std::uint64_t, last_update_frame, static_cast<std::uint64_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, shadow_map_shader_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, shader_index, static_cast<std::uint32_t>(-1));

public:
    ShadowCasterDirectional(core::ecs::Entity* entity, std::string&& name);
    ~ShadowCasterDirectional() override;
    void set_shadow_map(std::shared_ptr<render::texture::Texture2D>&& t, core::job::EndCaller<>&& end_callback) override;
    void set_shadow_map_target(std::shared_ptr<render::texture::Target>&& t) override;
    void after_record(std::uint64_t frame_number, const render::record::ShadowCasterDirectionalLightData& rec);
};
}
#endif