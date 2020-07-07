#include "gx-demo-wwr-garage.hpp"
#include "gx-demo-wwr-rotating-button.hpp"
#include "gx-demo-wwr-training.hpp"
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
#include <gearoenix/render/scene/rnd-scn-logo.hpp>
#include <gearoenix/render/camera/rnd-cmr-arc-controller.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/widget/rnd-wdg-button.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/physics/constraint/phs-cns-manager.hpp>
#include <gearoenix/physics/constraint/phs-cns-window-placer.hpp>
#include <gearoenix/physics/constraint/phs-cns-window-scaler.hpp>
#include <gearoenix/physics/collider/phs-cld-collider.hpp>
#include <gearoenix/system/sys-app.hpp>

void gearoenix::demo::wwr::Garage::initialize_scenes(
        const std::vector<std::shared_ptr<render::scene::Scene>> &scenes) noexcept {
#ifdef GX_DEBUG_MODE
    if(scenes.size() != 2) GX_UNEXPECTED
#endif
    for(const auto& s: scenes) {
        switch (s->get_scene_type()) {
            case render::scene::Type::Game:
                game_scene = s;
                break;
            case render::scene::Type::Ui:
                ui_scene = s;
                break;
            default:
            GX_UNEXPECTED
        }
    }
}

void gearoenix::demo::wwr::Garage::initialize_camera() noexcept {
    cam_ctrl = std::make_unique<render::camera::ArcController>(game_scene->get_cameras().begin()->second);
    cam_ctrl->set_max_vertical_angle(0.8);
    cam_ctrl->set_min_vertical_angle(0.2);
    cam_ctrl->set_vertical_angle(0.4);
    cam_ctrl->set_max_distance(6.0);
    cam_ctrl->set_min_distance(4.0);
    cam_ctrl->set_distance(5.0);
    cam_ctrl->set_target(math::Vec3(0.0, 0.0, 0.0));
}

void gearoenix::demo::wwr::Garage::initialize_buttons() noexcept {
    auto *const cns_mgr = ui_scene->get_e()->get_system_application()->get_asset_manager()->get_constraint_manager();

    shop_button = std::make_unique<RotatingButton>(std::dynamic_pointer_cast<render::widget::Button>(ui_scene->get_model("button-shop")));
    multiplayer_button = std::make_unique<RotatingButton>(std::dynamic_pointer_cast<render::widget::Button>(ui_scene->get_model("button-multiplayer")));
    tuning_button = std::make_unique<RotatingButton>(std::dynamic_pointer_cast<render::widget::Button>(ui_scene->get_model("button-tuning")));

    training_button = std::make_unique<RotatingButton>(std::dynamic_pointer_cast<render::widget::Button>(ui_scene->get_model("button-training")));
    training_button->get_button()->set_on_click([this] { load_training_scene(); });

    auto bottom_button = ui_scene->get_model("bottom-garage-buttons");

    bottom_button_scaler = cns_mgr->create<physics::constraint::WindowScaler>("bottom_button_scaler");
    bottom_button_scaler->set_size_percentage(0.8);
    bottom_button_scaler->set_max_size_scale(2.5);
    bottom_button_scaler->add_affected(bottom_button);
    bottom_button_scaler->set_current_size(2.0);

    auto bottom_button_placer = cns_mgr->create<physics::constraint::WindowPlacer>("bottom_button_placer");
    bottom_button_placer->set_width_percentage(0.5);
    bottom_button_placer->add_affected(bottom_button);
    bottom_button_placer->set_distance([bottom_button { move(bottom_button) }] {
        return math::Vec2(0.0, bottom_button->get_collider()->get_current_local_scale().y * 1.2);
    });

    bottom_button_scaler->add(bottom_button_placer);
    bottom_button_scaler->register_listener();

    next_button_placer = cns_mgr->create<physics::constraint::WindowPlacer>("next_button_placer");
    next_button_placer->set_width_percentage(1.0);
    next_button_placer->set_height_percentage(0.5);
    next_button_placer->add_affected(ui_scene->get_model("button-next"));
    next_button_placer->set_distance(math::Vec2(-0.1, 0.0));

    previous_button_placer = cns_mgr->create<physics::constraint::WindowPlacer>("previous_button_placer");
    previous_button_placer->set_height_percentage(0.5);
    previous_button_placer->add_affected(ui_scene->get_model("button-previous"));
    previous_button_placer->set_distance(math::Vec2(0.1, 0.0));

    player_name_placer = cns_mgr->create<physics::constraint::WindowPlacer>("player_name_placer");
    player_name_placer->set_height_percentage(1.0);
    player_name_placer->add_affected(ui_scene->get_model("text-player-name"));
    player_name_placer->set_distance(math::Vec2(0.05, -0.05));

    score_placer = cns_mgr->create<physics::constraint::WindowPlacer>("score_placer");
    score_placer->set_width_percentage(1.0);
    score_placer->set_height_percentage(1.0);
    score_placer->add_affected(ui_scene->get_model("text-money"));
    score_placer->set_distance(math::Vec2(-0.05, -0.05));
}

void gearoenix::demo::wwr::Garage::load_training_scene() noexcept {
    logo_scene = render::scene::Logo::construct(
            system_application,
            { {1.0, "game"}, {1.1, "game-hud"} },
            [this] (const std::vector<std::shared_ptr<render::scene::Scene>>& s) {
                training = std::make_unique<Training>(s);
                logo_scene = nullptr;
            });
}

gearoenix::demo::wwr::Garage::Garage(const std::vector <std::shared_ptr<render::scene::Scene>> &scenes) noexcept
    : system_application(scenes[0]->get_e()->get_system_application())
{
    initialize_scenes(scenes);
    initialize_camera();
    initialize_buttons();
}

gearoenix::demo::wwr::Garage::~Garage() noexcept = default;
