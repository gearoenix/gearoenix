#include "gx-gl-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::gl::Model, gearoenix::gl::Model::MAX_COUNT>::construct();
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::gl::Model::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types<render::model::Model>(this);
    return types;
}

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

std::shared_ptr<gearoenix::gl::Model> gearoenix::gl::Model::Model::construct(
    Engine& e,
    std::vector<std::shared_ptr<render::mesh::Mesh>>&& meshes,
    std::string&& name,
    const bool is_transformable,
    const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(e, std::move(meshes), std::move(name), is_transformable, entity_id);
    self->set_component_self(self);
    return self;
}

gearoenix::gl::Model::~Model() = default;

gearoenix::gl::ModelBuilder::ModelBuilder(
    Engine& e,
    std::string&& name,
    physics::TransformationComponent* const parent_transform,
    std::vector<std::shared_ptr<render::mesh::Mesh>>&& meshes,
    core::job::EndCaller<>&& end_caller,
    const bool is_transformable)
    : Builder(e, name, parent_transform, meshes, std::move(end_caller))
    , e(e)
{
    entity_builder->get_builder().add_component(Model::construct(
        e, std::move(meshes), std::move(name), is_transformable, entity_builder->get_id()));
}

gearoenix::gl::ModelBuilder::~ModelBuilder() = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::gl::ModelManager::build(
    std::string&& name,
    physics::TransformationComponent* parent_transform,
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
}

gearoenix::gl::ModelManager::~ModelManager() = default;

#endif
