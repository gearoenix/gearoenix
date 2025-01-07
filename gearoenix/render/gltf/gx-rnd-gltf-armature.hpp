#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <map>
#include <memory>

namespace gearoenix::physics::animation {
struct Armature;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::gltf {
struct Context;
struct Armatures final {
    const Context& context;

    std::map<int, std::shared_ptr<physics::animation::Armature>> map;

    explicit Armatures(const Context& context);
    ~Armatures();
    [[nodiscard]] bool is_armature(int node_index) const;
    [[nodiscard]] bool process(
        int node_index,
        physics::Transformation* parent_transform,
        const core::job::EndCaller<>& gpu_end_callback,
        const core::job::EndCaller<>& entity_end_callback,
        const std::shared_ptr<scene::Builder>& scene_builder) const;
};
}