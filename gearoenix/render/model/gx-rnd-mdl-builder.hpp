#ifndef GEAROENIX_RENDER_MODEL_BUILDER_HPP
#define GEAROENIX_RENDER_MODEL_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
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
struct Builder {
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);

protected:
    Builder(
        engine::Engine& e,
        const std::string& name,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        std::shared_ptr<material::Material>&& bound_material,
        core::sync::EndCaller&& end_caller,
        bool is_transformable = true) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder() noexcept;

    [[nodiscard]] physics::Transformation& get_transformation() noexcept;
    [[nodiscard]] core::ecs::entity_id_t get_id() const noexcept;
};
}
#endif