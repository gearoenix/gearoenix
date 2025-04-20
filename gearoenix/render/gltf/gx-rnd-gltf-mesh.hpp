#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include <vector>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::gltf {
struct Context;
struct Mesh;
struct Meshes final {
    const Context& context;
    std::vector<std::unique_ptr<Mesh>> meshes;

    explicit Meshes(const Context& context);
    ~Meshes();
    void load(core::job::EndCaller<>&&);
    [[nodiscard]] bool is_mesh(int node_index) const;
    [[nodiscard]] bool process(int node_index, core::ecs::Entity* parent) const;
};
}
