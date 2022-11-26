#ifndef GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#define GEAROENIX_RENDER_MATERIAL_UNLIT_HPP
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Unlit : public Material {
    GX_GET_REF_PRT(math::Vec4<float>, albedo_factor);
    GX_GETSET_VAL_PRT(float, alpha_cutoff, 0.0);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, albedo);

protected:
    Unlit(engine::Engine& e, const std::string& name, const core::sync::EndCaller& c) noexcept;

public:
    ~Unlit() noexcept override;
    void set_albedo(const std::shared_ptr<texture::Texture2D>& t) noexcept override;
};
}
#endif
