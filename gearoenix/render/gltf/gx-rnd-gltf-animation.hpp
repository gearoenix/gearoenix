#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>

namespace gearoenix::physics::animation {
struct BoneBuilder;
}

namespace gearoenix::render::gltf {
struct Context;
struct Animations final {
    const Context& context;

    boost::container::flat_map<int /*bone-node-index*/, std::shared_ptr<physics::animation::BoneBuilder>> bone_builders;

    explicit Animations(const Context& context);
    ~Animations();
    void load(const core::job::EndCaller<>& end);
    [[nodiscard]] int find_parent(int) const;
    [[nodiscard]] physics::animation::BoneBuilder* get(int, const core::job::EndCaller<>& end);
    [[nodiscard]] bool is_bone(int) const;
};
}