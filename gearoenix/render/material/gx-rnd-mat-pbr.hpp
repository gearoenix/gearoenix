#ifndef GEAROENIX_RENDER_MATERIAL_PBR_HPP
#define GEAROENIX_RENDER_MATERIAL_PBR_HPP
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Pbr : public Material {
    GX_GET_REF_PRT(math::Vec4<float>, albedo_factor);
    GX_GET_REF_PRT(math::Vec4<float>, emission_roughness_factor);
    GX_GET_REF_PRT(math::Vec4<float>, normal_metallic_factor);
    GX_GET_REF_PRT(math::Vec4<float>, alpha_cutoff_occlusion_strength_reserved_reserved);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, albedo);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, normal);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, emission);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, metallic_roughness);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, occlusion);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, brdflut);

protected:
    Pbr(engine::Engine& e, const std::string& name, const core::sync::EndCaller& c) noexcept;

public:
    ~Pbr() noexcept override;
    void set_albedo(const std::shared_ptr<texture::Texture2D>&) noexcept override;
    virtual void set_normal(std::shared_ptr<texture::Texture2D>&&) noexcept;
    virtual void set_emission(std::shared_ptr<texture::Texture2D>&&) noexcept;
    virtual void set_metallic_roughness(std::shared_ptr<texture::Texture2D>&&) noexcept;
    virtual void set_occlusion(std::shared_ptr<texture::Texture2D>&&) noexcept;
};
}

#endif