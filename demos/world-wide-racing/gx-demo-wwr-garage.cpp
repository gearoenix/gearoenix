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
}

gearoenix::demo::wwr::Garage::~Garage() noexcept = default;
