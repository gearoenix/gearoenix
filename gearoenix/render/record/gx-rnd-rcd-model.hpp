#pragma once
#include "../../core/gx-cr-growing-array.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include <boost/container/static_vector.hpp>
#include <vector>

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

struct ModelThreadData final {
    std::vector<Model> static_models;
    std::vector<Model> dynamic_models;
    std::size_t static_models_hash = 0;

    void clear();
};

struct Models final {
    std::vector<ModelThreadData> threads;
    std::vector<Model> static_models;
    std::vector<Model> dynamic_models;
    physics::accelerator::Bvh<Model> static_models_bvh;
    physics::accelerator::Bvh<Model> dynamic_models_bvh;
    std::uintptr_t previous_frame_static_models_hash = 0;

    Models();
    void update(core::ecs::Entity* scene_entity);
};
}