#include "gx-vk-rfl-baked.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../texture/gx-vk-txt-cube.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::vulkan::reflection::Baked::Baked(
    core::ecs::Entity* const entity,
    std::string&& name,
    std::shared_ptr<render::texture::TextureCube>&& irr,
    std::shared_ptr<render::texture::TextureCube>&& rad,
    const math::Aabb3<double>& include_box)
    : render::reflection::Baked(entity, core::ecs::ComponentType::create_index(this), std::move(irr), std::move(rad), include_box, std::move(name))
{
    set_textures(
        *core::cast_ptr<texture::TextureCube>(irradiance.get()),
        *core::cast_ptr<texture::TextureCube>(radiance.get()));
}

gearoenix::vulkan::reflection::Baked::~Baked() = default;

#endif