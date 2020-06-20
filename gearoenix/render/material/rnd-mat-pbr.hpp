#ifndef GEAROENIX_RENDER_MATERIAL_PBR_HPP
#define GEAROENIX_RENDER_MATERIAL_PBR_HPP
#include "../../math/math-vector-4d.hpp"
#include "rnd-mat-material.hpp"

namespace gearoenix::render::material {
class Pbr final : public Material {
public:
    struct Uniform {
        float alpha = 1.0f;
        float alpha_cutoff = 0.001f;
        math::Vec3<float> emission_factor = math::Vec3(0.999999f);
        float metallic_factor = 0.999999f;
        float normal_scale = 1.0f;
        float occlusion_strength = 1.0f;
        float roughness_factor = 0.999999f;
        float radiance_lod_coefficient = 0.0f;
    };
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, color_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec4<float>>, color_value)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, emission_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec3<float>>, emission_value)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, metallic_roughness_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec2<float>>, metallic_roughness_value)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, normal_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec3<float>>, normal_value)
    GX_GETSET_PTR_PRV(texture::TextureCube, irradiance)
    GX_GET_PTR_PRV(texture::TextureCube, radiance)
    GX_GET_CREF_PRV(Uniform, uniform)
public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    Pbr(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    Pbr(const Pbr& o) noexcept;
    ~Pbr() noexcept final;
    void update() noexcept final;
    [[nodiscard]] Material* clone() const noexcept final;
    void set_metallic_factor(float f) noexcept;
    void set_roughness_factor(float f) noexcept;
    void set_color(std::uint32_t code, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    void set_color(
        float r, float g, float b,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    void set_color(const math::Vec4<float>& c,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_color(const std::shared_ptr<texture::Texture2D>& color) noexcept;
    void set_alpha(float) noexcept;
    void set_radiance(texture::TextureCube*) noexcept;
};
}

#endif