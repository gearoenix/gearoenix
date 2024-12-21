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
struct Lights final {
    const Context& context;

    explicit Lights(const Context& context);
    ~Lights();
    [[nodiscard]] bool is_light(int node_index) const;
    [[nodiscard]] bool process(
        int node_index, physics::Transformation* parent_transform,
        const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder) const;
};
}