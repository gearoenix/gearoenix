#ifndef GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#define GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Unlit final : public Material {
public:
    struct Uniform {
        float alpha = 1.0f;
        float alpha_cutoff = 0.001f;
    };
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, color_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec4<float>>, color_value)
    GX_GET_CREF_PRV(Uniform, uniform)
public:
    Unlit(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    Unlit(platform::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    ~Unlit() noexcept final;
    void update() noexcept final;
    void set_color(
        float r, float g, float b,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_color(const math::Vec4<float>& c,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& end = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_color(const std::shared_ptr<texture::Texture2D>& color) noexcept;
    void set_alpha(float) noexcept;
};
}
#endif
