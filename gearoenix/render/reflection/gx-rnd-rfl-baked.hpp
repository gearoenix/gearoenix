#ifndef GEAROENIX_RENDER_REFLECTION_BAKED_HPP
#define GEAROENIX_RENDER_REFLECTION_BAKED_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-aabb.hpp"
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Baked final : public core::ecs::Component {
    GX_GET_VAL_PRV(std::size_t, radiance_mips_count, 0);
    GX_GET_CREF_PRV(math::Aabb3<double>, include_box);
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, irradiance);
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, radiance);
    GX_GETSET_VAL_PRV(core::ecs::entity_id_t, scene_id, 0);
    GX_GET_RRF_PRV(engine::Engine, e);

public:
    Baked(
        engine::Engine& e,
        std::shared_ptr<texture::TextureCube> irradiance,
        std::shared_ptr<texture::TextureCube> radiance,
        math::Aabb3<double> include_box) noexcept;
    ~Baked() noexcept final;
    Baked(Baked&&) noexcept;
};
}
#endif