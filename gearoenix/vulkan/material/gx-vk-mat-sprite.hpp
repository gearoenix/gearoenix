#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/material/gx-rnd-mat-sprite.hpp"
#include "gx-vk-mat-material.hpp"
#include "../descriptor/gx-vk-des-uniform-holder.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"

namespace gearoenix::vulkan::material {
struct Sprite final : render::material::Sprite, Material {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = render::material::Sprite::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_material_sprite_type_index;
    constexpr static std::array all_parent_object_type_indices { render::material::Material::object_type_index, render::material::Sprite::object_type_index, material::Material::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::material::Sprite::object_type_index, material::Material::object_type_index };

    const descriptor::UniformHolder<GxShaderDataMaterial>::PtrHolder shader_data;

private:
    explicit Sprite(std::string&& name);

public:
    static void construct(std::string&& name, core::job::EndCallerShared<render::material::Sprite>&& c);
    ~Sprite() override;
    void set_albedo(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_albedo_factor(const math::Vec4<float>&) override;
    void set_uv_transform(const math::Vec4<float>&) override;
    void set_alpha_cutoff(float) override;
    void bind_forward(VkCommandBuffer cmd, bool skinned, VkPipeline& current_bound_pipeline) override;
    void bind_shadow(VkCommandBuffer cmd, bool skinned, VkPipeline& current_bound_pipeline) override;
};
}
#endif
