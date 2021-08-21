#ifndef GEAROENIX_RENDER_SCENE_BUILDER_HPP
#define GEAROENIX_RENDER_SCENE_BUILDER_HPP
#include <memory>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct EntitySharedBuilder;
}

namespace gearoenix::render::camera {
struct Builder;
}

namespace gearoenix::render::mesh {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder final {
    friend struct Manager;

private:
    const std::shared_ptr<core::ecs::EntitySharedBuilder> entity_builder;
    std::vector<std::shared_ptr<mesh::Builder>> mesh_builders;
    std::vector<std::shared_ptr<camera::Builder>> camera_builders;

    explicit Builder(core::ecs::World& world) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    ~Builder() noexcept;

    void add(std::shared_ptr<mesh::Builder>&& mesh_builder) noexcept;
    void add(std::shared_ptr<camera::Builder>&& camera_builder) noexcept;
};
}

#endif