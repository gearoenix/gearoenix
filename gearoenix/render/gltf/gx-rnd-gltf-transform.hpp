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
void apply_transform(int node_index, const Context&, physics::Transformation&);
[[nodiscard]] bool has_transformation(int node_index, const Context&);
[[nodiscard]] physics::Transformation* create_empty_entity_transform(
    int node_index,
    const Context&,
    physics::Transformation* parent_transform,
    const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder);
}