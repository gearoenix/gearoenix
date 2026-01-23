#include "gx-vk-mat-pbr.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../texture/gx-vk-txt-2d.hpp"

gearoenix::vulkan::material::Pbr::Pbr(std::string&& name)
    : render::material::Pbr(object_type_index, std::move(name))
{
}

gearoenix::vulkan::material::Pbr::~Pbr() = default;

void gearoenix::vulkan::material::Pbr::show_debug_gui()
{
    render::material::Pbr::show_debug_gui();
}

void gearoenix::vulkan::material::Pbr::set_albedo(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    albedo_texture_index = t.get_view_index();
    albedo_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_albedo(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_normal(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    normal_texture_index = t.get_view_index();
    normal_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_albedo(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_emission(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    emission_texture_index = t.get_view_index();
    emission_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_albedo(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_metallic_roughness(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    metallic_roughness_texture_index = t.get_view_index();
    metallic_roughness_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_albedo(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_occlusion(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    occlusion_texture_index = t.get_view_index();
    occlusion_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_albedo(std::move(txt));
}

#endif