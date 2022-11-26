#ifndef GEAROENIX_GL_MATERIAL_PBR_HPP
#define GEAROENIX_GL_MATERIAL_PBR_HPP
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
struct Pbr final : public render::material::Pbr, public Material {
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
    Pbr(Engine& e, const std::string& name, const core::sync::EndCaller& c) noexcept;
    ~Pbr() noexcept final;
    void shadow(const submission::Model& model, const submission::Camera& camera, uint& current_shader) noexcept final;
    void forward_render(const submission::Model& model, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) noexcept final;
    void deferred_gbuffer_render(const submission::Model& model, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) noexcept final;
    void set_albedo(const std::shared_ptr<render::texture::Texture2D>&) noexcept final;
    void set_normal(std::shared_ptr<render::texture::Texture2D>&&) noexcept final;
    void set_emission(std::shared_ptr<render::texture::Texture2D>&&) noexcept final;
    void set_metallic_roughness(std::shared_ptr<render::texture::Texture2D>&&) noexcept final;
    void set_occlusion(std::shared_ptr<render::texture::Texture2D>&&) noexcept final;
};
}

#endif
#endif