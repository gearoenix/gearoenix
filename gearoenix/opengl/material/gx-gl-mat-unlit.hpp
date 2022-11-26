#ifndef GEAROENIX_GL_MATERIAL_UNLIT_HPP
#define GEAROENIX_GL_MATERIAL_UNLIT_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-unlit.hpp"
#include "gx-gl-material.hpp"

namespace gearoenix::gl::shader {
struct ShadowCasterCombination;
struct UnlitCombination;
}

namespace gearoenix::gl::material {
struct Unlit final : public render::material::Unlit, public Material {
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_albedo);
    GX_GET_REFC_PRV(std::shared_ptr<shader::ShadowCasterCombination>, shadow_caster_combination);
    GX_GET_REFC_PRV(std::shared_ptr<shader::UnlitCombination>, unlit_combination);

public:
    Unlit(Engine& e, const std::string& name, const core::sync::EndCaller& c) noexcept;
    ~Unlit() noexcept final;
    void shadow(const submission::Model& model, const submission::Camera& camera, uint& current_shader) noexcept final;
    void forward_render(const submission::Model& model, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) noexcept final;
    void deferred_gbuffer_render(const submission::Model& model, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) noexcept final;
    void set_albedo(const std::shared_ptr<render::texture::Texture2D>&) noexcept final;
};
}

#endif
#endif