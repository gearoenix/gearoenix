#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-pbr.hpp"
#include "gx-gl-material.hpp"

namespace gearoenix::gl::shader {
struct ShadowCasterCombination;
struct ForwardPbrCombination;
struct GBuffersFiller;
}

namespace gearoenix::gl::material {
struct Pbr final : render::material::Pbr, Material {
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_albedo);
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_normal);
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_emission);
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_metallic_roughness);
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_occlusion);
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_brdflut);
    GX_GET_REFC_PRV(std::shared_ptr<shader::ShadowCasterCombination>, shadow_caster_combination);
    GX_GET_REFC_PRV(std::shared_ptr<shader::ForwardPbrCombination>, forward_pbr_combination);
    GX_GET_REFC_PRV(std::shared_ptr<shader::GBuffersFiller>, gbuffers_filler_combination); /// TODO it has too change to combination and support bones, ...

public:
    static void construct(const std::string& name, core::job::EndCallerShared<render::material::Pbr>&& c);
    explicit Pbr(const std::string& name);
    ~Pbr() override;
    void shadow(
        const Mesh& mesh, const render::record::Camera& camera, const render::record::CameraModel&, uint& current_shader) override;
    void render_forward(
        const Scene& scene, const render::record::Camera& camera, const render::record::CameraModel&,
        const Mesh& mesh, uint& current_shader) override;
    // void deferred_gbuffer_render(const Model& model, const Mesh& mesh, const Camera& camera, const Scene& scene, uint& current_shader) override;
    void set_albedo(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_normal(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_emission(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_metallic_roughness(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_occlusion(std::shared_ptr<render::texture::Texture2D>&&) override;
    void set_brdflut(std::shared_ptr<render::texture::Texture2D>&&) override;
};
}

#endif