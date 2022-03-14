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
struct Pbr final {
    GX_GET_CREF_PRV(math::Vec4<float>, albedo_factor)
    GX_GET_CREF_PRV(math::Vec4<float>, emission_roughness_factor)
    GX_GET_CREF_PRV(math::Vec4<float>, normal_metallic_factor)
    GX_GET_VAL_PRV(float, alpha_cutoff, 0.0f)
    GX_GET_VAL_PRV(float, occlusion_strength, 1.0f)
    GX_GET_VAL_PRV(float, radiance_lod_coefficient, 0.0f)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, albedo)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, normal__metallic)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, emission__roughness)

public:
    Pbr(engine::Engine& e, const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    ~Pbr() noexcept;

    Pbr(const Pbr&) noexcept;
    Pbr(Pbr&&) noexcept;
    Pbr& operator=(Pbr&&) noexcept;
    Pbr& operator=(const Pbr&) noexcept;
};
}

#endif