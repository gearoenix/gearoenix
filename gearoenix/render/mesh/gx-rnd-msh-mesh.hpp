#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"

namespace gearoenix::core::ecs {
struct World;
}

namespace gearoenix::render::mesh {
struct Builder;
struct Mesh final : public core::ecs::Component {
    std::type_index material_type_index;
    math::Aabb3 original_box;
    /// It is going to be used in BVH creation, picker and ...
    math::Aabb3 updated_box;
};
}
#endif