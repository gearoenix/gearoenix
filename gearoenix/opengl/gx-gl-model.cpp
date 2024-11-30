#include "gx-gl-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"

gearoenix::gl::Model::Model(
    Engine&,
    std::vector<std::shared_ptr<render::mesh::Mesh>>&& bound_meshes,
    std::string&& name,
    const bool is_transformable,
    const core::ecs::entity_id_t entity_id)
    : render::model::Model(
          create_this_type_index(this),
          is_transformable,
          std::move(bound_meshes),
          std::move(name),
          entity_id)
{
    gl_meshes.reserve(meshes.size());
    for (const auto& mesh : meshes) {
        gl_meshes.emplace_back(std::dynamic_pointer_cast<Mesh>(mesh));
    }
}

gearoenix::gl::Model::~Model() = default;

gearoenix::gl::ModelBuilder::ModelBuilder(
    Engine& e,
    std::string&& name,
    physics::Transformation* const parent_transform,
    std::vector<std::shared_ptr<render::mesh::Mesh>>&& meshes,
    core::job::EndCaller<>&& end_caller,
    const bool is_transformable)
    : Builder(e, name, parent_transform, meshes, std::move(end_caller))
    , e(e)
{
    entity_builder->get_builder().add_component(Model::construct<Model>(
        e, std::move(meshes), std::move(name), is_transformable, entity_builder->get_id()));
}

gearoenix::gl::ModelBuilder::~ModelBuilder() = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::gl::ModelManager::build(
    std::string&& name,
    physics::Transformation* parent_transform,
    std::vector<std::shared_ptr<render::mesh::Mesh>>&& meshes,
    core::job::EndCaller<>&& end_caller,
    const bool is_transformable)
{
    return std::shared_ptr<render::model::Builder>(new ModelBuilder(
        dynamic_cast<Engine&>(e), std::move(name), parent_transform, std::move(meshes), std::move(end_caller), is_transformable));
}

gearoenix::gl::ModelManager::ModelManager(Engine& e)
    : Manager(e)
{
    Model::register_type<Model>();
}

gearoenix::gl::ModelManager::~ModelManager() = default;

#endif
