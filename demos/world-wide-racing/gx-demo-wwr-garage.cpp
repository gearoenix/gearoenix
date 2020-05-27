#include "gx-demo-wwr-garage.hpp"
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
#include <gearoenix/render/camera/rnd-cmr-arc-controller.hpp>

gearoenix::demo::wwr::Garage::Garage(const std::vector <std::shared_ptr<render::scene::Scene>> &scenes) noexcept {
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
    cam_ctrl = std::make_unique<render::camera::ArcController>(game_scene->get_cameras().begin()->second);
    cam_ctrl->set_max_vertical_angle(0.8);
    cam_ctrl->set_min_vertical_angle(0.0);
    cam_ctrl->set_vertical_angle(0.4);
    cam_ctrl->set_max_distance(6.0);
    cam_ctrl->set_min_distance(4.0);
    cam_ctrl->set_distance(5.0);
    cam_ctrl->set_target(math::Vec3(0.0, 0.0, 1.0));
}

gearoenix::demo::wwr::Garage::~Garage() noexcept = default;
