#pragma once
#include "../../math/gx-math-frustum.hpp"

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::accelerator {
struct Bvh;
}

namespace gearoenix::render::light {
struct ShadowCasterDirectional;
}

namespace gearoenix::render::record {
struct Model;
struct Models;

struct ShadowCasterDirectionalLightData final {
    const math::Frustum<core::fp_t>* const frustum;
    const math::Mat4x4<float>* const normalised_vp;
    const math::Vec3<core::fp_t>* const direction;
    const math::Vec3<float>* const colour;
    const core::ecs::Entity* const entity;
    light::ShadowCasterDirectional* const shadow_caster_directional;
    const physics::Transformation* const transform;
};

struct Lights final {
    std::vector<ShadowCasterDirectionalLightData> shadow_caster_directionals;

    void update_per_frame(core::ecs::Entity* scene_entity);
    void update_after_change(core::ecs::Entity* scene_entity);
    void update_models(physics::accelerator::Bvh& bvh, std::vector<Model>& models);
    void update_dynamic_models(Models& models);
    void update_static_models(Models& models);
};
}