#include "gx-gl-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../render/material/gx-rnd-mat-material.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "material/gx-gl-material.hpp"

namespace {
gearoenix::core::allocator::SharedArray<gearoenix::gl::Model, gearoenix::gl::Model::MAX_COUNT> allocator;
}

const boost::container::flat_set<std::type_index>& gearoenix::gl::Model::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { create_type_index<render::model::Model>(), create_this_type_index(this) };
    return types;
}

gearoenix::gl::Model::Model(
    Engine&,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    std::shared_ptr<render::material::Material>&& mat,
    std::string&& name,
    const bool is_transformable)
    : render::model::Model(
          create_this_type_index(this),
          is_transformable,
          std::move(mesh),
          std::move(mat),
          std::move(name))
    , gl_mesh(std::dynamic_pointer_cast<Mesh>(bound_mesh))
    , gl_material(std::dynamic_pointer_cast<material::Material>(bound_material))
{
}

std::shared_ptr<gearoenix::gl::Model> gearoenix::gl::Model::Model::construct(
    Engine& e,
    std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
    std::shared_ptr<render::material::Material>&& mat,
    std::string&& name,
    const bool is_transformable)
{
    auto self = allocator.make_shared(e, std::move(bound_mesh), std::move(mat), std::move(name), is_transformable);
    self->set_component_self(self);
    return self;
}

gearoenix::gl::Model::~Model() = default;

gearoenix::gl::ModelBuilder::ModelBuilder(
    Engine& e,
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    std::shared_ptr<render::material::Material>&& mat,
    core::job::EndCaller<>&& end_caller,
    const bool is_transformable)
    : render::model::Builder(e, name, mesh, std::move(end_caller))
    , e(e)
{
    entity_builder->get_builder().add_component(Model::construct(
        e, std::move(mesh), std::move(mat), std::move(name), is_transformable));
}

gearoenix::gl::ModelBuilder::~ModelBuilder() = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::gl::ModelManager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    std::shared_ptr<render::material::Material>&& material,
    core::job::EndCaller<>&& end_caller,
    const bool is_transformable)
{
    return std::shared_ptr<render::model::Builder>(new ModelBuilder(
        dynamic_cast<Engine&>(e), std::move(name), std::move(mesh),
        std::move(material), std::move(end_caller), is_transformable));
}

gearoenix::gl::ModelManager::ModelManager(Engine& e)
    : render::model::Manager(e)
{
}

gearoenix::gl::ModelManager::~ModelManager() = default;

#endif
