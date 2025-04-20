#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::gltf {
struct Context;
struct Lights final {
    const Context& context;

    explicit Lights(const Context& context);
    ~Lights();
    [[nodiscard]] bool is_light(int node_index) const;
    [[nodiscard]] bool process(
        int node_index,
        core::ecs::Entity* parent,
        const core::job::EndCaller<>& end_callback) const;
};
}