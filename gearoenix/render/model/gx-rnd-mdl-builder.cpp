#include "gx-rnd-mdl-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "gx-rnd-mdl-model.hpp"

gearoenix::render::model::Builder::Builder(
    render::engine::Engine& e,
    const std::string& name,
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    std::shared_ptr<material::Material>&& bound_material,
    core::sync::EndCaller&& end_caller,
    bool is_transformable) noexcept
    : entity_builder(e.get_world()->create_shared_builder(std::string(name), std::move(end_caller)))
{
    auto& builder = entity_builder->get_builder();
    builder.set_name(name);
    builder.add_component(physics::collider::Aabb3(bound_mesh->box, name + "-collider"));
    builder.add_component(physics::Transformation(name + "-transformation"));
    builder.add_component(Model(is_transformable, std::move(bound_mesh), std::move(bound_material), name + "-model"));
}

gearoenix::render::model::Builder::~Builder() noexcept = default;

gearoenix::physics::Transformation& gearoenix::render::model::Builder::get_transformation() noexcept
{
    return *entity_builder->get_builder().get_component<physics::Transformation>();
}

gearoenix::core::ecs::entity_id_t gearoenix::render::model::Builder::get_id() const noexcept
{
    return entity_builder->get_id();
}
