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
struct Nodes final {
    const Context& context;

    explicit Nodes(const Context& context);
    ~Nodes();
    [[nodiscard]] bool is_empty(int node_index) const;
    [[nodiscard]] bool process_empty(int node_index, core::ecs::Entity* parent, const core::job::EndCaller<>& end_callback) const;
    void process(int node_index, core::ecs::Entity* parent, const core::job::EndCaller<>& end_callback) const;
};
}