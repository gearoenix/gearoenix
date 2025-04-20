#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-sky-types.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::skybox {
struct Manager {
protected:
    Manager();

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager();

    [[nodiscard]] virtual core::ecs::EntityPtr build(
        std::string&& name,
        core::ecs::Entity* parent,
        Texture&& bound_texture,
        std::shared_ptr<mesh::Mesh>&& bound_mesh)
        = 0;

    void build(
        std::string&& name,
        core::ecs::Entity* parent,
        const platform::stream::Path& texture_path,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);

    void build(
        std::string&& name,
        core::ecs::Entity* parent,
        Texture&& bound_texture,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);
};
}