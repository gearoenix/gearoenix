export module gearoenix.render.record.skybox;

#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-static-flat-map.hpp"
#include "../skybox/gx-rnd-sky-skybox.hpp"

namespace gearoenix::render::record {
export struct Skyboxes final {

    struct Data final {
        core::ecs::Entity* entity = nullptr;
        skybox::Skybox* skybox = nullptr;
        bool equirectangular = false;
    };

    core::static_flat_map<double, Data, 8> skyboxes;

    void update(core::ecs::Entity* const scene_entity)
    {
        skyboxes.clear();
        core::ecs::World::get().synchronised_system<skybox::Skybox>([&](auto* const entity, const auto* const skybox) {
            if (!skybox->get_enabled() || !entity->contains_in_parents(scene_entity)) {
                return;
            }
            skyboxes.insert(
                skybox->get_layer(),
                Data {
                    .entity = entity,
                    .skybox = skybox,
                    .equirectangular = skybox->is_equirectangular(),
                });
        });
    }
};
}