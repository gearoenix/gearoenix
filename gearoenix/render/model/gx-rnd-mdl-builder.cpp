#include "gx-rnd-mdl-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-buffer.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "gx-rnd-mdl-model.hpp"

gearoenix::render::model::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const std::vector<std::shared_ptr<mesh::Mesh>>& bound_meshes,
    core::job::EndCaller<>&& end_caller)
    : entity_builder(e.get_world()->create_shared_builder(std::string(name), std::move(end_caller)))
{
    auto& builder = entity_builder->get_builder();
    math::Aabb3<double> box;
    for (const auto& m : bound_meshes) {
        box.put(m->get_buffer()->get_box());
    }
    builder.add_component(physics::collider::Aabb3::construct(box, name + "-collider"));
    builder.add_component(physics::TransformationComponent::construct(name + "-transformation"));
}

gearoenix::render::model::Builder::~Builder() = default;

gearoenix::physics::TransformationComponent& gearoenix::render::model::Builder::get_transformation() const
{
    return *entity_builder->get_builder().get_component<physics::TransformationComponent>();
}

gearoenix::render::model::Model& gearoenix::render::model::Builder::get_model() const
{
    return *entity_builder->get_builder().get_component<Model>();
}

gearoenix::core::ecs::entity_id_t gearoenix::render::model::Builder::get_id() const
{
    return entity_builder->get_id();
}
