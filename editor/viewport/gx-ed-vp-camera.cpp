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
#include <gearoenix/render/gizmo/gx-rnd-gzm-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

gearoenix::editor::viewport::Camera::Camera(Application& app)
    : app(app)
    , e(app.get_render_engine())
{
    app.get_render_engine().get_camera_manager()->build(
        "Geareonix/Editor/Viewport/Camera", nullptr,
        core::job::EndCallerShared<render::camera::Builder>([this](auto&& camera_builder) {
            camera = camera_builder->get_camera().get_camera_self().lock();
            transformation = std::dynamic_pointer_cast<physics::Transformation>(camera_builder->get_transformation().get_component_self().lock());
            transformation->local_look_at({ 10, 10, 10 }, { 0, 0, 0 }, { 0, 0, 1 });
            (void)this->app.get_render_engine().get_physics_engine()->get_constraint_manager()->create_jet_controller(
                "Geareonix/Editor/Viewport/Camera/JetController",
                std::shared_ptr(transformation),
                core::job::EndCaller([] { }));
        }),
        core::job::EndCaller([] {}));
}

gearoenix::editor::viewport::Camera::~Camera() = default;

void gearoenix::editor::viewport::Camera::update()
{
    auto& w = *core::ecs::World::get();
    if (camera) {
        if (core::ecs::invalid_entity_id == camera->get_scene_id()) {
            w.synchronised_system<render::scene::Scene>([this](const auto scene_id, auto* const scene) {
                if (!scene->get_enabled()) {
                    return;
                }
                camera->set_scene_id(scene_id);
                scene->add_camera(camera->get_entity_id(), *camera);

                core::ecs::World::get()->synchronised_system<render::camera::Camera>([this, scene_id](const auto camera_id, auto* const c) {
                    if (camera_id == camera->get_entity_id()) {
                        return;
                    }

                    if (render::camera::Camera::Usage::Main != c->get_usage()) {
                        return;
                    }

                    c->set_enabled(false);
                });
            });
        }
        e.get_gizmo_manager()->set_viewport_camera(camera.get());
        e.get_gizmo_manager()->show_view();
    }
}
