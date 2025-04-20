#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace gearoenix::render::gltf {
struct Context;
struct Cameras final {
    const Context& context;

    explicit Cameras(const Context& context);
    ~Cameras();
    [[nodiscard]] bool is_camera(int node_index) const;
    [[nodiscard]] bool process(int node_index, core::ecs::Entity* parent, const core::job::EndCaller<>& end_callback) const;
};
}