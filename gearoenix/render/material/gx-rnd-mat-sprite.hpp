#ifndef GEAROENIX_RENDER_MATERIAL_SPRITE_HPP
#define GEAROENIX_RENDER_MATERIAL_SPRITE_HPP
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Sprite : public Material {
    GX_GET_REF_PRT(math::Vec4<float>, albedo_factor);
    GX_GET_REF_PRT(math::Vec4<float>, uv_transform);
    GX_GETSET_VAL_PRT(float, alpha_cutoff, 0.0);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, albedo);

protected:
    Sprite(engine::Engine& e, const std::string& nme, const core::sync::EndCaller& c) noexcept;

public:
    ~Sprite() noexcept override;
    void set_albedo(const std::shared_ptr<texture::Texture2D>& t) noexcept override;
};
}
#endif
