#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Pbr : Material {
    constexpr static auto max_count = 8192; // we need this number because of example 004 but for your need change it

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

    explicit Pbr(const std::string& name);
    void initialise(core::job::EndCallerShared<Pbr>&& c);

public:
    ~Pbr() override;
    void set_albedo(std::shared_ptr<texture::Texture2D>&&) override;
    virtual void set_normal(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_emission(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_metallic_roughness(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_occlusion(std::shared_ptr<texture::Texture2D>&&);
    virtual void set_brdflut(std::shared_ptr<texture::Texture2D>&&);
};
}