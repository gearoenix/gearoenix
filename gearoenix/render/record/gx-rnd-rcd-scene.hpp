#pragma once
#include "gx-rnd-rcd-camera.hpp"
#include "gx-rnd-rcd-light.hpp"
#include "gx-rnd-rcd-model.hpp"
#include "gx-rnd-rcd-reflection.hpp"
#include "gx-rnd-rcd-skybox.hpp"

#include <functional>

namespace gearoenix::render::record {
struct Scene final {
    Cameras cameras;
    Lights lights;
    Models models;
    Reflections reflections;
    Skyboxes skyboxes;

    std::array<std::function<void(core::ecs::Entity* scene_entity)>, 5> parallel_functions_1;

    Scene();
    ~Scene();

    void update_per_frame(core::ecs::Entity* scene_entity);
    void update_after_change(core::ecs::Entity* scene_entity);
};
}