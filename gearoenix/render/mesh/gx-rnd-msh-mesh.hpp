#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::mesh {
struct Mesh : public core::ecs::Component {
protected:
    std::shared_ptr<Mesh> self;

public:
    virtual void set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept = 0;
};
}
#endif
