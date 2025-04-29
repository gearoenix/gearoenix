#pragma once
#include "../../core/gx-cr-static-flat-map.hpp"

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::render::skybox {
struct Skybox;
}

namespace gearoenix::render::record {
struct Skyboxes final {
    struct Data final {
        core::ecs::Entity* entity = nullptr;
        skybox::Skybox* skybox = nullptr;
        bool equirectangular = false;
    };

    core::static_flat_map<double, Data, 8> skyboxes;

    void update(core::ecs::Entity* scene_entity);
};
}