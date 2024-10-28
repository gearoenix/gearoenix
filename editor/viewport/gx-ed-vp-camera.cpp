#include "gx-ed-vp-camera.hpp"
#include "../gx-editor-main.hpp"
#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

gearoenix::editor::viewport::Camera::Camera(Application& app)
    : app(app)
{
    app.get_render_engine().get_camera_manager()->build(
        "Geareonix/Editor/Viewport/Camera", nullptr,
        core::job::EndCallerShared<render::camera::Builder>([this](auto&& camera_builder) {
            camera = camera_builder->get_camera().get_camera_self().lock();
            transformation = std::dynamic_pointer_cast<physics::TransformationComponent>(camera_builder->get_transformation().get_component_self().lock());
            (void)this->app.get_render_engine().get_physics_engine()->get_constraint_manager()->create_jet_controller(
                "Geareonix/Editor/Viewport/Camera/JetController",
                transformation,
                core::job::EndCaller([] { }));
        }),
        core::job::EndCaller([] {}));
}

gearoenix::editor::viewport::Camera::~Camera() = default;

void gearoenix::editor::viewport::Camera::update()
{
    if (core::ecs::INVALID_ENTITY_ID == camera->get_scene_id()) {
        app.get_render_engine().get_world()->synchronised_system<render::scene::Scene>([this](const auto id, auto* const scene) {
            if (!scene->get_enabled()) {
                return;
            }
            camera->set_scene_id(id);
            scene->add_camera(camera->get_entity_id(), *camera);
        });
    }
}
