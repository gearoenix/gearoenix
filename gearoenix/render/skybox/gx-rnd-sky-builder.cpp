#include "gx-rnd-sky-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "gx-rnd-sky-skybox.hpp"

gearoenix::render::skybox::Builder::Builder(
    std::string&& name,
    core::job::EndCaller<>&& entity_end_callback)
    : entity_builder(std::make_shared<core::ecs::EntitySharedBuilder>(std::move(name), std::move(entity_end_callback)))
{
}

gearoenix::render::skybox::Builder::~Builder() = default;

gearoenix::core::ecs::entity_id_t gearoenix::render::skybox::Builder::get_entity_id() const
{
    return entity_builder->get_id();
}

const gearoenix::render::skybox::Skybox* gearoenix::render::skybox::Builder::get_skybox() const
{
    return entity_builder->get_builder().get_component<Skybox>();
}

gearoenix::render::skybox::Skybox* gearoenix::render::skybox::Builder::get_skybox()
{
    return entity_builder->get_builder().get_component<Skybox>();
}
