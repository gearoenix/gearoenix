#ifndef GEAROENIX_RENDER_MODEL_MANAGER_HPP
#define GEAROENIX_RENDER_MODEL_MANAGER_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <string>

namespace gearoenix::physics {
struct TransformationComponent;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Builder;
struct Manager {
protected:
    engine::Engine& e;

    explicit Manager(engine::Engine& e);

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager();

    [[nodiscard]] virtual std::shared_ptr<Builder> build(
        std::string&& name,
        physics::TransformationComponent* parent_transform,
        std::vector<std::shared_ptr<mesh::Mesh>>&& bound_meshes,
        core::job::EndCaller<>&& c,
        bool is_transformable)
        = 0;
};
}
#endif