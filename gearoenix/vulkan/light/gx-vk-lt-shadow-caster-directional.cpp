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
}

gearoenix::vulkan::light::ShadowCasterDirectional::~ShadowCasterDirectional() = default;

void gearoenix::vulkan::light::ShadowCasterDirectional::set_shadow_map(std::shared_ptr<render::texture::Texture2D>&& t, core::job::EndCaller<>&& end_callback)
{
    vk_shadow_map_texture = std::dynamic_pointer_cast<texture::Texture2D>(t);
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

#endif