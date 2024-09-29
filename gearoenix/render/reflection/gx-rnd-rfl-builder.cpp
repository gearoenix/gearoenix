#include "gx-rnd-rfl-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Builder::Builder(
    engine::Engine& e, std::string&& name,
    physics::TransformationComponent* parent_transform,
    core::job::EndCaller<>&& end_callback)
    : transformation(physics::TransformationComponent::construct(name + "-transform", parent_transform))
    , entity_builder(e.get_world()->create_shared_builder(std::move(name), std::move(end_callback)))
{
    auto& builder = entity_builder->get_builder();
    builder.add_component(transformation);
}

gearoenix::render::reflection::Builder::~Builder() = default;

const gearoenix::render::reflection::Runtime& gearoenix::render::reflection::Builder::get_runtime() const
{
    const auto* const c = entity_builder->get_builder().get_component<Runtime>();
    GX_ASSERT(nullptr != c);
    return *c;
}

gearoenix::render::reflection::Runtime& gearoenix::render::reflection::Builder::get_runtime()
{
    auto* const c = entity_builder->get_builder().get_component<Runtime>();
    GX_ASSERT(nullptr != c);
    return *c;
}

void gearoenix::render::reflection::Builder::set_camera_builder(std::shared_ptr<camera::Builder>&& builder, const std::size_t face_index)
{
    faces_camera_builders[face_index] = std::move(builder);
}

gearoenix::core::ecs::entity_id_t gearoenix::render::reflection::Builder::get_id() const
{
    return entity_builder->get_id();
}
