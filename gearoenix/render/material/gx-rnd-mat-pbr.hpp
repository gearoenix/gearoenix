#ifndef GEAROENIX_RENDER_MATERIAL_PBR_HPP
#define GEAROENIX_RENDER_MATERIAL_PBR_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-vector-4d.hpp"

namespace gearoenix::render::material {
struct Pbr final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Vec4<float>, colour_factor)
    GX_GET_CREF_PRV(math::Vec4<float>, emission_factor__alpha_cutoff)
    GX_GET_CREF_PRV(math::Vec4<float>, normal_scale__occlusion_strength)
    GX_GET_CREF_PRV(math::Vec4<float>, metallic_factor__roughness_factor__radiance_lod_coefficient)

public:
    Pbr() noexcept
        : core::ecs::Component(this)
    {
    }
    ~Pbr() noexcept final = default;

    Pbr(const Pbr&) = delete;
    Pbr(Pbr&&) noexcept = default;
    Pbr& operator=(Pbr&&) = delete;
    Pbr& operator=(const Pbr&) = delete;
};
}

#endif