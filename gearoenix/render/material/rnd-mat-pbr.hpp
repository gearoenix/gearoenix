#ifndef GEAROENIX_RENDER_MATERIAL_PBR_HPP
#define GEAROENIX_RENDER_MATERIAL_PBR_HPP
#include "rnd-mat-material.hpp"

namespace gearoenix::render::material {
class Pbr final : public Material {
public:
    struct Uniform {
        core::Real alpha = 1.0f;
        core::Real alpha_cutoff = 0.001f;
        core::Real metallic_factor = 0.9f;
        core::Real normal_scale = 1.0f;
        core::Real occlusion_strength = 1.0f;
        core::Real roughness_factor = 0.1f;
    };
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, color_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec4>, color_value)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, emission_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec3>, emission_value)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, metallic_roughness_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec2>, metallic_roughness_value)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, normal_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec3>, normal_value)
    GX_GET_CREF_PRV(Uniform, uniform)
public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    Pbr(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    ~Pbr() noexcept final;
    void update() noexcept final;
    void set_metallic_factor(core::Real f) noexcept;
    void set_roughness_factor(core::Real f) noexcept;
    void set_color(
        core::Real r, core::Real g, core::Real b,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    void set_color(const math::Vec4& c,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_color(const std::shared_ptr<texture::Texture2D>& color) noexcept;
    void set_alpha(core::Real) noexcept;
};
}

#endif