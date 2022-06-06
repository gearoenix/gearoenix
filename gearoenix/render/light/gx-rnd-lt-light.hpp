#ifndef GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::render::light {
struct Light final : public core::ecs::Component {
    math::Vec3<float> colour = { 1.0f, 1.0f, 1.0f };
    core::ecs::Entity::id_t scene_id = 0;
    bool is_shadow_caster = false;

    Light() noexcept;
    Light(Light&&) noexcept;
    ~Light() noexcept final;
};
}
#endif
