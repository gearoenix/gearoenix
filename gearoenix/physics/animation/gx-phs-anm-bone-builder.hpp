#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Bone;
struct BoneBuilder final {
    const std::shared_ptr<core::ecs::EntitySharedBuilder> builder;
    const std::shared_ptr<Transformation> transform;
    const std::shared_ptr<Bone> bone;

    BoneBuilder(std::string&& name, Bone* parent, core::job::EndCaller<>&& end_caller);
    ~BoneBuilder();
};
}