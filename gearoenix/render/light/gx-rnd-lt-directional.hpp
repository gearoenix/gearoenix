#ifndef GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target;
struct Texture2D;
}

namespace gearoenix::render::light {
struct Builder;
struct Directional final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Vec3<float>, direction);

public:
    Directional() noexcept;
    Directional(Directional&&) noexcept;
    ~Directional() noexcept final;
};

struct ShadowCasterDirectional final : public core::ecs::Component {
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, shadow_map);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, shadow_map_target);
    GX_GET_VAL_PRV(core::ecs::Entity::id_t, camera_id, 0);

public:
    ShadowCasterDirectional(
        const std::string& name,
        std::size_t resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        engine::Engine&,
        Builder& builder,
        const core::sync::EndCallerIgnored& end_callback) noexcept;
    ShadowCasterDirectional(ShadowCasterDirectional&&) noexcept;
    ~ShadowCasterDirectional() noexcept final;
};
}

#endif
