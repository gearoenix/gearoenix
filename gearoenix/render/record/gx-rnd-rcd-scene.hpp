#pragma once
#include "gx-rnd-rcd-camera.hpp"
#include "gx-rnd-rcd-light.hpp"
#include "gx-rnd-rcd-model.hpp"
#include "gx-rnd-rcd-reflection.hpp"
#include "gx-rnd-rcd-skybox.hpp"

namespace gearoenix::render::record {
struct Scene final {
    Cameras cameras;
    Lights lights;
    Models models;
    Reflections reflections;
    Skyboxes skyboxes;

    void update(core::ecs::Entity* scene_entity);
};
}