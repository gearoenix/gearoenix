#ifndef GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::physics::constraint {
struct Constraint;
struct Manager final {
    GX_GET_RRF_PRV(render::engine::Engine, e);

public:
    explicit Manager(render::engine::Engine& e);
    ~Manager();
    [[nodiscard]] std::shared_ptr<core::ecs::EntitySharedBuilder> create_jet_controller(
        std::string&& name,
        std::shared_ptr<Transformation>&& transform,
        core::ecs::entity_id_t entity_id,
        core::job::EndCaller<>&& entity_exists_in_world) const;
    void update();
};
}
#endif