#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

#include <string>

namespace gearoenix::render::scene {
struct Manager : core::Singleton<Manager> {
protected:
    Manager();

public:
    ~Manager() override;

    /// By layer, you decide in what order scenes to be rendered on top of each other
    virtual void build(std::string&& name, core::fp_t layer, core::job::EndCaller<core::ecs::EntityPtr>&& end) const = 0;
    virtual void update();
};
}
