#ifndef GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#define GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#include "rnd-mat-material.hpp"

namespace gearoenix::render::material {
class Unlit : public Material {
public:
    struct Uniform {
        core::Real alpha = 1.0f;
        core::Real alpha_cutoff = 0.001f;
    };
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, color_texture)
    GX_GET_CREF_PRV(std::optional<math::Vec4>, color_value)
    GX_GET_CREF_PRV(Uniform, uniform)
public:
    Unlit(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    Unlit(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    ~Unlit() noexcept final;
    void update() noexcept final;
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
