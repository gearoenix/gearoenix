#include "gx-vk-lt-shadow-caster-directional.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/record/gx-rnd-rcd-light.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-target.hpp"
#include "gx-vk-lt-manager.hpp"

gearoenix::vulkan::light::ShadowCasterDirectional::ShadowCasterDirectional(core::ecs::Entity* const entity, std::string&& name)
    : render::light::ShadowCasterDirectional(entity, core::ecs::ComponentType::create_index(this), std::move(name))
{
    static_assert(GX_VK_SHD_MAX_SHADOW_CASTER_DIRECTIONAL_LIGHTS == GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER, "Change shader to support all the expected lights.");
    static_assert(0 == GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER % 4, "Lights count must be multiply of 4, it is necessary for the alignment.");
}

gearoenix::vulkan::light::ShadowCasterDirectional::~ShadowCasterDirectional() = default;

void gearoenix::vulkan::light::ShadowCasterDirectional::set_shadow_map(
    std::shared_ptr<render::texture::Texture2D>&& t,
    core::job::EndCaller<>&& end_callback)
{
    vk_shadow_map_texture = std::dynamic_pointer_cast<texture::Texture2D>(t);
    shadow_map_shader_index = vk_shadow_map_texture->get_view_index();
    render::light::ShadowCasterDirectional::set_shadow_map(std::move(t), std::move(end_callback));
}

void gearoenix::vulkan::light::ShadowCasterDirectional::set_shadow_map_target(std::shared_ptr<render::texture::Target>&& t)
{
    vk_shadow_map_target = std::dynamic_pointer_cast<texture::Target>(t);
    // It must be only 2D
    GX_ASSERT_D(vk_shadow_map_target->get_gapi_attachments()[0].texture.index() == texture::Target::GapiAttachment::VARIANT_2D_INDEX);
    vk_shadow_map_texture = std::get<texture::Target::GapiAttachment::VARIANT_2D_INDEX>(vk_shadow_map_target->get_gapi_attachments()[0].texture);
    render::light::ShadowCasterDirectional::set_shadow_map_target(std::move(t));
}

void gearoenix::vulkan::light::ShadowCasterDirectional::after_record(const std::uint64_t frame_number, const render::record::ShadowCasterDirectionalLightData& rec)
{
    if (last_update_frame == frame_number) {
        return;
    }
    last_update_frame = frame_number;

    auto sd = directional_shadow_casters_uniform_indexer_t::get().get_next();

    auto& [direction_bit_shadow_map_texture_index, colour, normalised_vp] = *sd.get_ptr();
    colour = math::Vec4(rec.colour, 0.0f);
    direction_bit_shadow_map_texture_index = math::Vec4(rec.direction, *reinterpret_cast<const float*>(&shadow_map_shader_index));
    normalised_vp = rec.normalised_vp;

    shader_index = sd.get_index();
}

#endif