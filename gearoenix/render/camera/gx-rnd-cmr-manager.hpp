#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <string>

namespace gearoenix::render::camera {
struct Builder;
struct Manager : core::Singleton<Manager> {
protected:
    Manager();

    static void build_impl(
        std::string&& name,
        core::ecs::Entity* parent,
        const core::job::EndCaller<core::ecs::EntityPtr>& entity_callback);

public:
    ~Manager() override;

    virtual void build(
        std::string&& name,
        core::ecs::Entity* parent,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);

    virtual void update();
    virtual void window_resized();
};
}