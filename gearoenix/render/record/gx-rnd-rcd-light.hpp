#pragma once
#include "../../math/gx-math-frustum.hpp"

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::accelerator {
template <typename T>
struct Bvh;
}

namespace gearoenix::render::light {
struct ShadowCasterDirectional;
}

namespace gearoenix::render::record {
struct Model;
struct Models;

struct ShadowCasterDirectionalLightData final {
    math::Frustum<double> frustum;
    math::Mat4x4<float> normalised_vp;
    math::Vec3<float> direction;
    math::Vec3<float> colour;
    core::ecs::Entity* entity = nullptr;
    light::ShadowCasterDirectional* shadow_caster_directional = nullptr;
    physics::Transformation* transform = nullptr;
};

struct LightThreadData final {
    std::vector<ShadowCasterDirectionalLightData> shadow_caster_directionals;

    void clear();
};

struct Lights final {
    std::vector<LightThreadData> threads;
    std::vector<ShadowCasterDirectionalLightData> shadow_caster_directionals;

    Lights();
    void update(core::ecs::Entity* scene_entity);
    void update_models(physics::accelerator::Bvh<Model>& bvh) const;
    void update_dynamic_models(Models& models) const;
    void update_static_models(Models& models) const;
};
}