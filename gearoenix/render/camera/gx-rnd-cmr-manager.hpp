#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <string>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Builder;
struct Manager {
protected:
    engine::Engine& e;

    explicit Manager(engine::Engine& e);

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    virtual ~Manager() = default;

    virtual void build(
        const std::string& name,
        physics::Transformation* parent_transform,
        core::job::EndCallerShared<Builder>&& builder_end_caller,
        core::job::EndCaller<>&& entity_end_caller)
        = 0;

    virtual void update();
    virtual void window_resized();
};
}