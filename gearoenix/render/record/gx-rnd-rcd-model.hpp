#pragma once
#include "../../core/gx-cr-build-configuration.hpp"
#include "../../core/gx-cr-growing-array.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../gx-rnd-constants.hpp"

#include <boost/container/static_vector.hpp>

#include <vector>

// #define GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS true

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
struct Model final {
    model::Model* model = nullptr;
    physics::collider::Collider* collider = nullptr;
    reflection::Probe* probe = nullptr;

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