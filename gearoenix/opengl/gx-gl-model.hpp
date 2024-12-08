#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../render/model/gx-rnd-mdl-builder.hpp"
#include "../render/model/gx-rnd-mdl-manager.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"

namespace gearoenix::gl {
struct Engine;
struct Mesh;

struct Model final : render::model::Model {
    constexpr static core::ecs::component_index_t TYPE_INDEX = 9;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES { render::model::Model::TYPE_INDEX };
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES { render::model::Model::TYPE_INDEX };

    GX_GET_CREF_PRV(std::vector<std::shared_ptr<Mesh>>, gl_meshes);

public:
    Model(
        Engine& e,
        std::vector<std::shared_ptr<render::mesh::Mesh>>&& bound_meshes,
        std::string&& name,
        bool is_transformable,
        core::ecs::entity_id_t entity_id);
    ~Model() override;
};

struct ModelBuilder final : render::model::Builder {
    friend struct ModelManager;

    Engine& e;

private:
    ModelBuilder(
        Engine& e,
        std::string&& name,
        physics::Transformation* parent_transform,
        std::vector<std::shared_ptr<render::mesh::Mesh>>&& meshes,
        core::job::EndCaller<>&& end_caller,
        bool is_transformable);

public:
    ~ModelBuilder() override;
};

struct ModelManager final : render::model::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(
        std::string&& name,
        physics::Transformation* parent_transform,
        std::vector<std::shared_ptr<render::mesh::Mesh>>&& meshes,
        core::job::EndCaller<>&& end_caller,
        bool is_transformable) override;

public:
    explicit ModelManager(Engine& e);
    ~ModelManager() override;
};
}

#endif
