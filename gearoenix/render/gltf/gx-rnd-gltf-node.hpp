#pragma once
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
    [[nodiscard]] bool process_empty(
        int node_index, physics::Transformation* parent_transform, const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback, const std::shared_ptr<scene::Builder>& scene_builder) const;
    void process(
        int node_index, physics::Transformation* parent_transform, const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback, const std::shared_ptr<scene::Builder>& scene_builder) const;
};
}
