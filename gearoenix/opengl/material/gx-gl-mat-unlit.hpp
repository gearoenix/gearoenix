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
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = render::material::Unlit::max_count;
    constexpr static auto object_type_index = gearoenix_gl_material_unlit_type_index;
    constexpr static std::array all_parent_object_type_indices { render::material::Material::object_type_index, render::material::Unlit::object_type_index, gl::material::Material::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { render::material::Unlit::object_type_index, gl::material::Material::object_type_index };

    GX_GET_CREF_PRV(std::shared_ptr<Texture2D>, gl_albedo);
    GX_GET_REFC_PRV(std::shared_ptr<shader::ShadowCasterCombination>, shadow_caster_combination);
    GX_GET_REFC_PRV(std::shared_ptr<shader::UnlitCombination>, unlit_combination);

    explicit Unlit(std::string&& name);

public:
    static void construct(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c);
    ~Unlit() override;
    void shadow(const Mesh&, const render::record::Camera&, const render::record::CameraModel&, uint&) override;
    void render_forward(const Scene&, const render::record::Camera&, const render::record::CameraModel&, const Mesh&, uint&) override;
    void set_albedo(std::shared_ptr<render::texture::Texture2D>&&) override;
};
}
#endif