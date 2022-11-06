#ifndef GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#define GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Unlit final : public Material {
    GX_GET_REF_PRV(math::Vec4<float>, albedo_factor);
    GX_GETSET_VAL_PRV(float, alpha_cutoff, 0.0);
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, albedo);

public:
    explicit Unlit(engine::Engine& e, const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    ~Unlit() noexcept final;
};
}
#endif
