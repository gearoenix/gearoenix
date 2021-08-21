#include "gx-rnd-scn-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../mesh/gx-rnd-msh-builder.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Builder::Builder(core::ecs::World& world) noexcept
    : entity_builder(world.create_shared_builder())
{
    entity_builder->get_builder().add_component(Scene());
}

gearoenix::render::scene::Builder::~Builder() noexcept = default;

void gearoenix::render::scene::Builder::add(std::shared_ptr<mesh::Builder>&& mesh_builder) noexcept
{
    entity_builder->get_builder().get_component<Scene>()->get().add_mesh(
        mesh_builder->get_entity_builder()->get_builder().get_id());
    mesh_builders.push_back(std::move(mesh_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<camera::Builder>&& camera_builder) noexcept
{
    entity_builder->get_builder().get_component<Scene>()->get().add_camera(
        camera_builder->get_entity_builder()->get_builder().get_id());
    camera_builders.push_back(std::move(camera_builder));
}
