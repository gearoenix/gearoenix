#ifndef GEAROENIX_RENDER_MODEL_BUILDER_HPP
#define GEAROENIX_RENDER_MODEL_BUILDER_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

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
struct Model;
struct Builder {
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);

    Builder(
        engine::Engine& e,
        const std::string& name,
        physics::TransformationComponent* parent_transform,
        const std::vector<std::shared_ptr<mesh::Mesh>>& bound_meshes,
        core::job::EndCaller<>&& end_caller);

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder();

    [[nodiscard]] physics::TransformationComponent& get_transformation() const;
    [[nodiscard]] Model& get_model() const;
    [[nodiscard]] core::ecs::entity_id_t get_id() const;
};
}
#endif