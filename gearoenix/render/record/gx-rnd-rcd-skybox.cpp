#include "gx-rnd-rcd-skybox.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../skybox/gx-rnd-sky-skybox.hpp"

void gearoenix::render::record::Skyboxes::update(core::ecs::Entity* const scene_entity)
{
    skyboxes.clear();
    core::ecs::World::get().synchronised_system<skybox::Skybox>([&](auto* const entity, auto* const skybox) {
        if (!skybox->get_enabled() || !entity->contains_in_parents(scene_entity)) {
            return;
        }
        skyboxes.emplace(skybox->get_layer(),
            Data {
                .entity = entity,
                .skybox = skybox,
                .equirectangular = skybox->is_equirectangular(),
            });
    });
}