#pragma once
#include "../../core/gx-cr-build-configuration.hpp"
#include "../../core/gx-cr-growing-array.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../gx-rnd-build-configuration.hpp"

#include <boost/container/static_vector.hpp>

#include <vector>

#define GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS true

#if GX_DEBUG_MODE
#define GEAROENIX_RENDER_RECORD_MODEL_DEBUG true
#endif

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Armature;
}

namespace gearoenix::physics::collider {
struct Collider;
}

namespace gearoenix::render::light {
struct Light;
}

namespace gearoenix::render::model {
struct Model;
}

namespace gearoenix::render::reflection {
struct Probe;
}

namespace gearoenix::render::record {
struct ShadowCasterDirectionalLightData;

struct ModelDirectionalLight final {
    math::Vec3<float> direction;
    light::Light* light = nullptr;

    ~ModelDirectionalLight() = default;
};

struct ModelLight final {
    core::GrowingArray<ModelDirectionalLight, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directionals;
    core::GrowingArray<ShadowCasterDirectionalLightData*, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directionals;

    ~ModelLight() = default;
};

struct Model final {
    core::ecs::Entity* entity = nullptr;
    model::Model* model = nullptr;
    physics::animation::Armature* armature = nullptr;
    physics::Transformation* transform = nullptr;
    physics::collider::Collider* collider = nullptr;
    reflection::Probe* probe = nullptr;
    std::uint32_t bones_count = 0;
    ModelLight lights;

    ~Model() = default;
};

struct Models final {
    std::vector<Model> models;
    std::size_t dynamic_models_starting_index = 0;
    physics::accelerator::Bvh static_models_bvh;

#if GEAROENIX_RENDER_RECORD_MODEL_DEBUG
    std::uint64_t last_frame_update = 0;
    std::size_t previous_models_hash = 0;
#endif

#if GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    physics::accelerator::Bvh dynamic_models_bvh;
#endif

    Models();
    void update_after_change(core::ecs::Entity* scene_entity);
    void update_per_frame(core::ecs::Entity* scene_entity);
};
}