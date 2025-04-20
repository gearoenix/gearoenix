#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <map>
#include <memory>

namespace gearoenix::physics::animation {
struct Armature;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::gltf {
struct Context;
struct Armatures final {
    const Context& context;

    std::map<int, std::shared_ptr<physics::animation::Armature>> map;

    explicit Armatures(const Context& context);
    ~Armatures();
    [[nodiscard]] bool is_armature(int node_index) const;
    [[nodiscard]] bool process(int node_index, core::ecs::Entity* parent, const core::job::EndCaller<>& end_callback) const;
};
}