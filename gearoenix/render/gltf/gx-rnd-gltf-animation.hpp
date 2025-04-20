#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include <boost/container/flat_map.hpp>

namespace gearoenix::render::gltf {
struct Context;
struct Animations final {
    const Context& context;

    boost::container::flat_map<int /*bone-node-index*/, core::ecs::EntityPtr> bone_entities;

    explicit Animations(const Context& context);
    ~Animations();
    void load();
    [[nodiscard]] int find_parent(int) const;
    [[nodiscard]] core::ecs::Entity* get(int);
    [[nodiscard]] bool is_bone(int) const;
};
}