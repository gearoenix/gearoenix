#include "gx-rnd-sky-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"

gearoenix::render::skybox::Builder::Builder(
    engine::Engine& e,
    std::string&& name,
    core::job::EndCaller<>&& entity_end_callback)
    : entity_builder(e.get_world()->create_shared_builder(std::move(name), std::move(entity_end_callback)))
{
}

gearoenix::render::skybox::Builder::~Builder() = default;