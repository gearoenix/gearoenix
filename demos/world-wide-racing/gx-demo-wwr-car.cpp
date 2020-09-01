#include "gx-demo-wwr-car.hpp"
#include <gearoenix/physics/body/gx-phs-bd-car.hpp>
#include <gearoenix/render/camera/rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/rnd-cmr-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/render/model/rnd-mdl-dynamic.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/rnd-scn-scene.hpp>
#include <gearoenix/system/sys-app.hpp>

void gearoenix::demo::wwr::Car::loaded() noexcept
{
    scene->add_model(body_model);
    scene->add_model(front_left_wheel_model);
    scene->add_model(front_right_wheel_model);
    scene->add_model(rear_left_wheel_model);
    scene->add_model(rear_right_wheel_model);
    scene->get_cameras().begin()->second->get_transformation()->look_at(
        body_model->get_transformation()->get_location(),
        math::Vec3(0.0, 0.0, 1.0));
}

gearoenix::demo::wwr::Car::Car(
    std::string n,
    std::shared_ptr<render::scene::Scene> s) noexcept
    : name(std::move(n))
    , scene(std::move(s))
{
    auto* const e = scene->get_e();
    auto* const mdl_mgr = e->get_system_application()->get_asset_manager()->get_model_manager();
    core::sync::EndCaller<core::sync::EndCallerIgnore> end([this] { loaded(); });
    core::sync::EndCaller<render::model::Model> mdl_end([end](const std::shared_ptr<render::model::Model>&) {});
    body_model = std::dynamic_pointer_cast<render::model::Dynamic>(mdl_mgr->get_gx3d(name, mdl_end));
    front_left_wheel_model = std::dynamic_pointer_cast<render::model::Dynamic>(
        mdl_mgr->get_gx3d(name + "-wheel-front-left", mdl_end));
    front_right_wheel_model = std::dynamic_pointer_cast<render::model::Dynamic>(
        mdl_mgr->get_gx3d(name + "-wheel-front-right", mdl_end));
    rear_left_wheel_model = std::dynamic_pointer_cast<render::model::Dynamic>(
        mdl_mgr->get_gx3d(name + "-wheel-rear-left", mdl_end));
    rear_right_wheel_model = std::dynamic_pointer_cast<render::model::Dynamic>(
        mdl_mgr->get_gx3d(name + "-wheel-rear-right", mdl_end));
    rpm_handle = std::dynamic_pointer_cast<render::model::Dynamic>(
        mdl_mgr->get_gx3d("speedometer-handle", mdl_end));
    physical_body = std::make_unique<physics::body::Car>(
        name, body_model, front_left_wheel_model, front_right_wheel_model,
        rear_left_wheel_model, rear_right_wheel_model);
}

gearoenix::demo::wwr::Car::~Car() noexcept = default;

void gearoenix::demo::wwr::Car::accelerator_pressed(const bool b) noexcept
{
    physical_body->set_acceleration_pedal_percentage(b ? 1.0 : 0.0);
    rpm_handle->get_transformation()->local_z_rotate((b ? max_rpm_handle_rotation : 0.0) - current_rpm_handle_rotation);
    current_rpm_handle_rotation = b ? max_rpm_handle_rotation : 0.0;
}