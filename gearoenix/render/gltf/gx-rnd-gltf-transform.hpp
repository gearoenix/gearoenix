#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::gltf {

struct Context;
void apply_transform(int node_index, const Context&, physics::Transformation&);
[[nodiscard]] bool has_transformation(int node_index, const Context&);
[[nodiscard]] core::ecs::EntityPtr create_empty_entity_transform(int node_index, const Context&, core::ecs::Entity* parent);
}