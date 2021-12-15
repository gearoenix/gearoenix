#ifndef GEAROENIX_RENDER_MATERIAL_PBR_HPP
#define GEAROENIX_RENDER_MATERIAL_PBR_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::material {
struct Pbr final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Vec4<float>, albedo_factor)
    GX_GET_CREF_PRV(math::Vec3<float>, emission_factor)
    GX_GET_CREF_PRV(math::Vec3<float>, normal_scale)
    GX_GET_VAL_PRV(float, alpha_cutoff, 0.0f)
    GX_GET_VAL_PRV(float, occlusion_strength, 1.0f)
    GX_GET_VAL_PRV(float, metallic_factor, 0.5f)
    GX_GET_VAL_PRV(float, roughness_factor, 0.5f)
    GX_GET_VAL_PRV(float, radiance_lod_coefficient, 0.0f)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, albedo)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, normal__metallic)
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, emission__roughness)

public:
    Pbr(engine::Engine& e, const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    ~Pbr() noexcept final;

    Pbr(const Pbr&) = delete;
    Pbr(Pbr&&) noexcept = default;
    Pbr& operator=(Pbr&&) = delete;
    Pbr& operator=(const Pbr&) = delete;
};
}

#endif