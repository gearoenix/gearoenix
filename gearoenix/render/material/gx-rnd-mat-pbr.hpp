#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Pbr : Material {
    /// We need this number because of example-004 but for your project change it.
    constexpr static auto max_count = 8192;
    constexpr static auto object_type_index = gearoenix_render_material_pbr_type_index;
    constexpr static std::array all_parent_object_type_indices { Material::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Material::object_type_index };

    GX_GET_CREF_PRT(math::Vec4<float>, albedo_factor);
    GX_GET_CREF_PRT(math::Vec4<float>, emission_roughness_factor);
    GX_GET_CREF_PRT(math::Vec4<float>, normal_metallic_factor);
    GX_GET_CREF_PRT(math::Vec4<float>, alpha_cutoff_occlusion_strength_reserved_reserved);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, albedo);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, normal);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, emission);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, metallic_roughness);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, occlusion);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, brdflut);

    Pbr(core::object_type_index_t final_type_index, std::string&& name);
    void initialise(core::job::EndCallerShared<Pbr>&& c);

public:
    ~Pbr() override;
    void set_albedo(std::shared_ptr<texture::Texture2D>&&) override;
    void show_debug_gui() override;
    virtual void set_normal(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_emission(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_metallic_roughness(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_occlusion(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_brdflut(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_albedo_factor(const math::Vec4<float>&);
    virtual void set_emission_roughness_factor(const math::Vec4<float>&);
    virtual void set_normal_metallic_factor(const math::Vec4<float>&);
    virtual void set_alpha_cutoff_occlusion_strength_reserved_reserved(const math::Vec4<float>&);
};
}