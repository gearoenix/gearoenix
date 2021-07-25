#include "gx-rnd-scn-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
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
    //////////////// TODO
}
