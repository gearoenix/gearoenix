#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include <boost/container/flat_map.hpp>
#include <mutex>

namespace gearoenix::render::gltf {
struct Context;
struct Animations final {
    const Context& context;

    std::recursive_mutex bone_entities_lock;
    boost::container::flat_map<int /*bone-node-index*/, core::ecs::EntityPtr> bone_entities;

    explicit Animations(const Context& context);
    ~Animations();
    void load();
    [[nodiscard]] int find_parent(int) const;
    [[nodiscard]] core::ecs::Entity* get(int);
    [[nodiscard]] bool is_bone(int) const;
};
}