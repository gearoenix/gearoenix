#ifndef GEAROENIX_RENDER_MATERIAL_PBR_HPP
#define GEAROENIX_RENDER_MATERIAL_PBR_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-vector-4d.hpp"

namespace gearoenix::render::material {
struct Pbr final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Vec4<float>, color_factor)
    GX_GET_CREF_PRV(math::Vec3<float>, emission_factor)
    GX_GET_CREF_PRV(math::Vec3<float>, normal_scale)
    GX_GETSET_VAL_PRV(float, alpha, 1.0f)
    GX_GETSET_VAL_PRV(float, alpha_cutoff, 0.001f)
    GX_GETSET_VAL_PRV(float, metallic_factor, 0.999f)
    GX_GETSET_VAL_PRV(float, occlusion_strength, 1.0f)
    GX_GETSET_VAL_PRV(float, roughness_factor, 0.999f)
    GX_GETSET_VAL_PRV(float, radiance_lod_coefficient, 0.0f)
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