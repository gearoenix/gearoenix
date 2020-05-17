#include "rnd-scn-logo.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/animation/phs-anm-manager.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-transformation.hpp"
#include "../texture/rnd-txt-target.hpp"
#include "rnd-scn-manager.hpp"
#include "rnd-scn-scene.hpp"
#include <utility>

void gearoenix::render::scene::Logo::on_load(const std::shared_ptr<Scene>& s) noexcept
{
    logo_scene = s;

    core::sync::EndCaller<model::Model> end([](const std::shared_ptr<model::Model>&) {});
    const auto body = model_manager->get_gx3d("gearoenix-logo", end);
    const auto gear = model_manager->get_gx3d("gearoenix-logo-gear", end);
    const auto glare = model_manager->get_gx3d("gearoenix-logo-glare", end);
    const auto right_wing = model_manager->get_gx3d("gearoenix-logo-right-wing", end);
    const auto left_wing = model_manager->get_gx3d("gearoenix-logo-left-wing", end);

    const auto strong_self = self.lock();

    auto* const anm_mgr = render_engine->get_physics_engine()->get_animation_manager();

    left_wing->get_transformation()->local_z_rotate(wing_rotation);
    right_wing->get_transformation()->local_z_rotate(-wing_rotation);

    body->get_transformation()->local_scale(scale);

    anm_mgr->add(std::make_shared<physics::animation::Animation>(
        [this, strong_self, gear, glare](const double, const double delta_time) noexcept {
            gear->get_transformation()->local_z_rotate(delta_time * gear_rotation_speed);
            glare->get_transformation()->local_z_rotate(delta_time * glare_rotation_speed);
        },
        std::numeric_limits<double>::max()));

    anm_mgr->add(std::make_shared<physics::animation::Animation>(
        [this, strong_self, left_wing, right_wing](const double, const double delta_time) noexcept {
            const auto rot = delta_time * wing_rotation_speed;
            wing_rotation += rot;
            left_wing->get_transformation()->local_z_rotate(rot);
            right_wing->get_transformation()->local_z_rotate(-rot);
        },
        wing_max_rotation / wing_rotation_speed,
        [this, left_wing, right_wing](double) {
            left_wing->get_transformation()->local_z_rotate(-wing_rotation);
            right_wing->get_transformation()->local_z_rotate(wing_rotation);
        }));

    anm_mgr->add(std::make_shared<physics::animation::Animation>(
        [this, strong_self, body](const double time_from_start, const double) noexcept {
            const auto current_scale = 1.0 - (1.0 - 1.0 / max_scale) * (1.0 - time_from_start / scale_duration);
            body->get_transformation()->local_scale(current_scale / scale);
            scale = current_scale;
        },
        scale_duration,
        [this, body](double) {
            body->get_transformation()->local_scale(1.0 / scale);
        }));

    s->get_cameras().begin()->second->get_target()->set_clear_color(math::Vec4(1.0f));

    s->set_enability(true);
}

gearoenix::render::scene::Logo::Logo(
    model::Manager* const model_manager,
    engine::Engine* const render_engine,
    PriorityIds next_scenes_priority_id,
    FinishCallback on_finished) noexcept
    : model_manager(model_manager)
    , render_engine(render_engine)
    , next_scenes_priority_id(std::move(next_scenes_priority_id))
    , on_finished(std::move(on_finished))
{
}

std::shared_ptr<gearoenix::render::scene::Logo> gearoenix::render::scene::Logo::construct(
    system::Application* const sys_app,
    PriorityIds next_scenes_priority_id,
    FinishCallback on_finished) noexcept
{

    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    auto* const scn_mgr = ast_mgr->get_scene_manager();

    std::shared_ptr<Logo> result(new Logo(
        mdl_mgr, sys_app->get_render_engine(), std::move(next_scenes_priority_id), std::move(on_finished)));
    result->self = result;

    scn_mgr->get_gx3d("gearoenix-logo", core::sync::EndCaller<Scene>([result](const std::shared_ptr<Scene>& s) {
        result->on_load(s);
    }));

    return result;
}
