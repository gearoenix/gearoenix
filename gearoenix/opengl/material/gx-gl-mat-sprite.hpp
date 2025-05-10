#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-sprite.hpp"
#include "gx-gl-material.hpp"

namespace gearoenix::gl::shader {
struct ShadowCasterCombination;
struct UnlitCombination;
}

namespace gearoenix::gl::material {
struct Sprite final : render::material::Sprite, Material {
    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_albedo);
    GX_GET_REFC_PRV(std::shared_ptr<shader::ShadowCasterCombination>, shadow_caster_combination);
    GX_GET_REFC_PRV(std::shared_ptr<shader::UnlitCombination>, unlit_combination);

public:
    explicit Sprite(const std::string& name);
    static void construct(const std::string& name, core::job::EndCallerShared<render::material::Sprite>&& c);
    ~Sprite() override;
    void shadow(const Mesh&, const render::record::Camera&, const render::record::CameraModel&, uint& current_shader) override;
    void render_forward(
        const Scene&, const render::record::Camera&, const render::record::CameraModel&, const Mesh&, uint& current_shader) override;
    void deferred_gbuffer_render(const Model&, const Mesh&, const Camera&, const Scene&, uint& current_shader) override;
    void set_albedo(std::shared_ptr<render::texture::Texture2D>&&) override;
};
}

#endif