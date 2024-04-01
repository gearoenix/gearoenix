#ifndef GEAROENIX_RENDER_MATERIAL_SPRITE_HPP
#define GEAROENIX_RENDER_MATERIAL_SPRITE_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Sprite : Material {
    static constexpr std::size_t MAX_COUNT = 64;

    GX_GET_REF_PRT(math::Vec4<float>, albedo_factor);
    GX_GET_REF_PRT(math::Vec4<float>, uv_transform);
    GX_GETSET_VAL_PRT(float, alpha_cutoff, 0.0);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, albedo);

    Sprite(engine::Engine& e, const std::string& name);
    void initialise(core::job::EndCallerShared<Sprite>&& c);

public:
    ~Sprite() override;
    void set_albedo(std::shared_ptr<texture::Texture2D>&& t) override;
};
}
#endif
