export module gearoenix.render.record.scene;

import gearoenix.render.record.camera;
import gearoenix.render.record.light;
import gearoenix.render.record.model;
import gearoenix.render.record.reflection;
import gearoenix.render.record.skybox;

#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"

import std.function;

namespace gearoenix::render::record {
export struct Scene final {
    Cameras cameras;
    Lights lights;
    Models models;
    Reflections reflections;
    Skyboxes skyboxes;

    void update(core::ecs::Entity* const scene_entity)
    {
        constexpr std::array par_fns = {
            std::function([&] { cameras.update(scene_entity); }),
            std::function([&] { lights.update(scene_entity); }),
            std::function([&] { models.update(scene_entity); }),
            std::function([&] { reflections.update(scene_entity); }),
            std::function([&] { skyboxes.update(scene_entity); }),
        };

        core::sync::ParallelFor::exec(par_fns.begin(), par_fns.end(), [&](auto& f) { f(); });
    }
};
}