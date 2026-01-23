#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/material/gx-rnd-mat-pbr.hpp"
#include "gx-vk-mat-material.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::material {
struct Pbr final : render::material::Pbr, Material {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = render::material::Pbr::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_material_pbr_type_index;
    constexpr static std::array all_parent_object_type_indices { render::material::Material::object_type_index, render::material::Pbr::object_type_index, material::Material::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::material::Pbr::object_type_index, material::Material::object_type_index };

    GX_GET_VAL_PRV(std::uint32_t, albedo_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, albedo_sampler_index, static_cast<std::uint32_t>(-1));

    GX_GET_VAL_PRV(std::uint32_t, normal_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, normal_sampler_index, static_cast<std::uint32_t>(-1));

    GX_GET_VAL_PRV(std::uint32_t, emission_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, emission_sampler_index, static_cast<std::uint32_t>(-1));

    GX_GET_VAL_PRV(std::uint32_t, metallic_roughness_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, metallic_roughness_sampler_index, static_cast<std::uint32_t>(-1));

    GX_GET_VAL_PRV(std::uint32_t, occlusion_texture_index, static_cast<std::uint32_t>(-1));
    GX_GET_VAL_PRV(std::uint32_t, occlusion_sampler_index, static_cast<std::uint32_t>(-1));

public:
    explicit Pbr(std::string&& name);
    ~Pbr() override;
    void show_debug_gui() override;
    void set_albedo(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_normal(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_emission(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_metallic_roughness(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_occlusion(std::shared_ptr<render::texture::Texture2D>&&) override;
    void bind_forward(pipeline::Pipeline*& pipeline) override;
    void bind_shadow(pipeline::Pipeline*& pipeline) override;
};
}
#endif