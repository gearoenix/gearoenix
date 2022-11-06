#ifndef GEAROENIX_RENDER_MODEL_MANAGER_HPP
#define GEAROENIX_RENDER_MODEL_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <string>

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

    explicit Manager(engine::Engine& e) noexcept;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager() noexcept;

    [[nodiscard]] virtual std::shared_ptr<Builder> build(
        std::string&& name,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        std::shared_ptr<material::Material>&& bound_material,
        const core::sync::EndCallerIgnored& c,
        bool is_transformable) noexcept
        = 0;
};
}
#endif