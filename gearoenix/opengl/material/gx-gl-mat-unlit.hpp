#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-unlit.hpp"
#include "gx-gl-material.hpp"

namespace gearoenix::gl::shader {
struct ShadowCasterCombination;
struct UnlitCombination;
}

namespace gearoenix::gl::material {
struct Unlit final : render::material::Unlit, Material {
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_albedo);
    GX_GET_REFC_PRV(std::shared_ptr<shader::ShadowCasterCombination>, shadow_caster_combination);
    GX_GET_REFC_PRV(std::shared_ptr<shader::UnlitCombination>, unlit_combination);

public:
    explicit Unlit(const std::string& name);
    static void construct(const std::string& name, core::job::EndCallerShared<render::material::Unlit>&& c);
    ~Unlit() override;
    void shadow(const submission::Model& model, const submission::Mesh& mesh, const submission::Camera& camera, uint& current_shader) override;
    void forward_render(const submission::Model& model, const submission::Mesh& mesh, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) override;
    void deferred_gbuffer_render(const submission::Model& model, const submission::Mesh& mesh, const submission::Camera& camera, const submission::Scene& scene, uint& current_shader) override;
    void set_albedo(std::shared_ptr<render::texture::Texture2D>&&) override;
};
}
#endif