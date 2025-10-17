#include "gx-ed-vp-camera.hpp"
#include "editor/ui/gx-ed-ui-menu-scene.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-utilities.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-jet-controller.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/gizmo/gx-rnd-gzm-manager.hpp>

void gearoenix::editor::viewport::Camera::update_camera()
{
    camera = nullptr;
    transformation = nullptr;

    const auto* const scene_entity = ui::MenuScene::get().get_current_scene();
    if (!scene_entity) {
        return;
    }

    const auto camera_search = core::ecs::find<render::camera::Camera>(*scene_entity);
    if (camera_search.empty()) {
        return;
    }

    for (const auto& camera_entity : camera_search | std::views::values) {
        if (!camera_entity->get_enabled()) {
            continue;
        }

        camera = camera_entity->get_component<render::camera::Camera>();
        if (!camera || !camera->get_enabled() || camera->get_usage() != render::camera::Camera::Usage::Main) {
            camera = nullptr;
            continue;
        }

        transformation = camera_entity->get_component<physics::Transformation>();
        if (!transformation || !transformation->get_enabled()) {
            camera = nullptr;
            transformation = nullptr;
            continue;
        }

        return;
    }
}

void gearoenix::editor::viewport::Camera::update_controller()
{
    if (camera == nullptr) {
        if (controller_entity.get()) {
            controller_entity->set_enabled(false);
            controller_component->set_enabled(false);
        }
        return;
    }

    if (controller_entity.get() == nullptr) {
        controller_entity = physics::constraint::Manager::get().create_jet_controller(
            "Geareonix/Editor/Viewport/Camera/JetController",
            core::cast_shared<physics::Transformation>(transformation->get_object_self().lock()),
            nullptr);
        controller_entity->add_to_world();
        controller_component = controller_entity->get_component_shared_ptr<physics::constraint::JetController>();
    }

    if (!controller_entity->get_enabled()) {
        controller_entity->set_enabled(true);
        controller_component->set_enabled(true);
    }

    if (controller_component->get_transformation().get() != transformation) {
        controller_component->set_transformation(core::cast_shared<physics::Transformation>(transformation->get_object_self().lock()));
    }
}

void gearoenix::editor::viewport::Camera::update_gizmo()
{
    if (!controller_entity.get() || !controller_entity->get_enabled()) {
        return;
    }

    auto& m = render::gizmo::Manager::get();
    m.set_viewport_camera(camera);
    m.show_view();
}

gearoenix::editor::viewport::Camera::Camera() = default;

gearoenix::editor::viewport::Camera::~Camera() = default;

void gearoenix::editor::viewport::Camera::update()
{
    update_camera();
    update_controller();
    update_gizmo();
}

bool gearoenix::editor::viewport::Camera::is_active() const
{
    return controller_entity.get() && controller_entity->get_enabled();
}
