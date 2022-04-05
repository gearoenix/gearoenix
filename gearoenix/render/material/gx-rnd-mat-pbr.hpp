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
enum struct AlphaMode {
    Opaque = 1,
    Transparent = 2,
};

struct Pbr final {
    GX_GET_REF_PRV(math::Vec4<float>, albedo_factor)
    GX_GET_REF_PRV(math::Vec4<float>, emission_roughness_factor)
    GX_GET_REF_PRV(math::Vec4<float>, normal_metallic_factor)
    GX_GET_REF_PRV(math::Vec4<float>, alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved)
    GX_GETSET_VAL_PRV(AlphaMode, alpha_mode, AlphaMode::Opaque)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, albedo)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, normal)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, emission)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, metallic_roughness)
    GX_GETSET_CREF_PRV(std::shared_ptr<texture::Texture2D>, occlusion)

public:
    Pbr(engine::Engine& e, const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    ~Pbr() noexcept;

    Pbr(const Pbr&) noexcept;
    Pbr(Pbr&&) noexcept;
    Pbr& operator=(Pbr&&) noexcept;
    Pbr& operator=(const Pbr&) noexcept;

    void randomise_albedo() noexcept;
};
}

#endif