#include "exm-hello-world.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/event/cr-ev-bt-mouse.hpp>
#include <gearoenix/core/event/cr-ev-mv-mouse.hpp>
#include <gearoenix/core/event/cr-ev-sys-system.hpp>
#include <gearoenix/core/event/cr-ev-ui-ui.hpp>
#include <gearoenix/physics/body/phs-bd-rigid.hpp>
#include <gearoenix/physics/constraint/phs-cns-tracker-spring-joint-spring.hpp>
#include <gearoenix/render/camera/rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/rnd-cmr-manager.hpp>
#include <gearoenix/render/camera/rnd-cmr-perspective.hpp>
#include <gearoenix/render/camera/rnd-cmr-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/render/graph/tree/rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/light/rnd-lt-directional.hpp>
#include <gearoenix/render/light/rnd-lt-manager.hpp>
#include <gearoenix/render/material/rnd-mat-material.hpp>
#include <gearoenix/render/mesh/rnd-msh-manager.hpp>
#include <gearoenix/render/mesh/rnd-msh-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/model/rnd-mdl-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-model.hpp>
#include <gearoenix/render/model/rnd-mdl-transformation.hpp>
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/render/scene/rnd-scn-scene.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <gearoenix/system/sys-log.hpp>

template <class T>
using GxEndCaller = gearoenix::core::sync::EndCaller<T>;

using GxEndCallerIgnore = gearoenix::core::sync::EndCallerIgnore;
using GxEndCallerIgnored = GxEndCaller<GxEndCallerIgnore>;
using GxGrPbr = gearoenix::render::graph::tree::Pbr;
using GxGrTree = gearoenix::render::graph::tree::Tree;
using GxMaterial = gearoenix::render::material::Material;
using GxMdManager = gearoenix::render::model::Manager;
using GxMdMesh = gearoenix::render::model::Mesh;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxModel = gearoenix::render::model::Model;
using GxVec3 = gearoenix::math::Vec3;
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;

GameApp::GameApp(const std::shared_ptr<gearoenix::system::Application>& sys_app)
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored endcall([this] { scn->enable_rendering(); });
    GxEndCaller<GxScene> scncall([endcall](std::shared_ptr<GxScene>) {});
    GxEndCaller<GxMesh> mshcall([endcall](std::shared_ptr<GxMesh>) {});
    GxEndCaller<GxModel> mdlcall([endcall](std::shared_ptr<GxModel>) {});

    rnd_eng->set_render_tree(std::shared_ptr<GxGrTree>(new GxGrPbr(rnd_eng, endcall)));
    const std::shared_ptr<gearoenix::core::asset::Manager>& astmgr = sys_app->get_asset_manager();
    scn = astmgr->get_scene_manager()->create<GxScene>(scncall);

    std::shared_ptr<GxPersCam> cam = astmgr->get_camera_manager()->create<GxPersCam>();
    camtrn = std::static_pointer_cast<GxCamTran>(cam->get_transformation());
    camtrn->look_at(GxVec3(20.0f, 20.0f, 10.0f), GxVec3(0.0f, 0.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f));
    scn->add_camera(cam);

    const std::shared_ptr<GxDirLight> light = astmgr->get_light_manager()->create<GxDirLight>();
    light->enable_shadowing();
    scn->add_light(light);
    const std::shared_ptr<GxDirLight> light1 = astmgr->get_light_manager()->create<GxDirLight>();
    light1->set_direction(GxVec3(1.0f, 0.0f, -1.0f).normalized());
    light1->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light1);
    const std::shared_ptr<GxDirLight> light2 = astmgr->get_light_manager()->create<GxDirLight>();
    light2->set_direction(GxVec3(-1.0f, 0.0f, -1.0f).normalized());
    light2->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light2);
    const std::shared_ptr<GxDirLight> light3 = astmgr->get_light_manager()->create<GxDirLight>();
    light3->set_direction(GxVec3(0.0f, 1.0f, -1.0f).normalized());
    light3->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light3);
    const std::shared_ptr<GxDirLight> light4 = astmgr->get_light_manager()->create<GxDirLight>();
    light4->set_direction(GxVec3(0.0f, -1.0f, -1.0f).normalized());
    light4->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light4);

    const std::shared_ptr<GxMesh> msh = astmgr->get_mesh_manager()->create_icosphere(mshcall);
    const std::shared_ptr<GxMdManager>& mdlmgr = astmgr->get_model_manager();
    for (gearoenix::core::Real y = -10.0f, roughness = 0.1f; y < 10.1f; y += 2.5f, roughness += 0.1f) {
        for (gearoenix::core::Real x = -10.0f, metallic = 0.1f; x < 10.1f; x += 2.5f, metallic += 0.1f) {
            const std::shared_ptr<GxMaterial> mat(new GxMaterial(rnd_eng, endcall));
            mat->set_roughness_factor(roughness);
            mat->set_metallic_factor(metallic);
            const std::shared_ptr<GxModel> mdl = mdlmgr->create<GxModel>(mdlcall);
            mdl->add_mesh(std::make_shared<GxMdMesh>(msh, mat));
            mdl->get_transformation()->set_location(GxVec3(x, y, 0.0f));
            scn->add_model(mdl);
        }
    }
}

GameApp::~GameApp() {}

void GameApp::update()
{
    camtrn->global_rotate(rnd_eng->get_delta_time() * 0.1f, GxVec3(0.0f, 0.0f, 1.0f));
}

void GameApp::terminate()
{
    gearoenix::core::Application::terminate();
    scn = nullptr;
    camtrn = nullptr;
}

//void GameApp::on_event(const gearoenix::core::event::Event& e)
//{
//    switch (e.get_type()) {
//    case gearoenix::core::event::Event::From::SYSTEM: {
//        const gearoenix::core::event::system::System& se = e.to_system();
//        switch (se.get_action()) {
//        case gearoenix::core::event::system::System::Action::RELOAD:
//            rndeng->load_scene(0, [this]() -> void {
//                cam = rndeng->get_scene(0)->get_current_camera();
//                rndeng->load_scene(1, [this]() -> void {});
//            });
//            break;
//        default:
//            break;
//        }
//        break;
//    }
//    case gearoenix::core::event::Event::From::BUTTON: {
//        const gearoenix::core::event::button::Button& be = e.to_button();
//        switch (be.get_type()) {
//        case gearoenix::core::event::button::Button::MOUSE: {
//            switch (be.get_key()) {
//            case gearoenix::core::event::button::Button::LEFT:
//                switch (be.get_action()) {
//                case gearoenix::core::event::button::Button::PRESS:
//                    if (be.is_taken())
//                        break;
//                    left_mouse_down = true;
//                    break;
//                case gearoenix::core::event::button::Button::RELEASE:
//                    left_mouse_down = false;
//                    break;
//                default:
//                    break;
//                }
//                break;
//            default:
//                break;
//            }
//            break;
//        }
//        case gearoenix::core::event::button::Button::KEYBOARD:
//            switch (be.get_key()) {
//            case gearoenix::core::event::button::Button::LEFT:
//                mdl->local_z_rotate(0.1f);
//                break;
//            case gearoenix::core::event::button::Button::RIGHT:
//                mdl->local_z_rotate(-0.1f);
//                break;
//            case gearoenix::core::event::button::Button::UP:
//                mdl->local_y_translate(-0.1f);
//                break;
//            default:
//                break;
//            }
//            break;
//        default:
//            break;
//        }
//        break;
//    }
//    case gearoenix::core::event::Event::From::MOVEMENT: {
//        const gearoenix::core::event::movement::Movement& me = e.to_movement();
//        switch (me.get_type()) {
//        case gearoenix::core::event::movement::Movement::MOUSE: {
//            const gearoenix::core::event::movement::Mouse& mme = me.to_mouse();
//            if (left_mouse_down) {
//                cam->local_x_rotate(mme.get_dy() * 0.1f);
//                cam->global_rotate(mme.get_dx() * 0.1f, gearoenix::math::Vec3::Z);
//            }
//            break;
//        }
//        default:
//            break;
//        }
//        break;
//    }
//    case gearoenix::core::event::Event::From::UI: {
//        const gearoenix::core::event::ui::Ui& ue = e.to_ui();
//        if (ue.get_action() == gearoenix::core::event::ui::Ui::ActionType::CLICKED) {
//            if (ue.get_widget() == 19) {
//                rndeng->load_scene(0, [this]() -> void {
//                    const auto& scene = rndeng->get_scene(0);
//                    cam = scene->get_current_camera();
//                    scene->add_model(1);
//                    rndeng->delete_scene(1);
//                    rndeng->delete_scene(2);
//                    const std::shared_ptr<gearoenix::core::asset::Manager>& astmgr = rndeng->get_system_application()->get_asset_manager();
//                    scene->add_constraint(
//                        std::shared_ptr<gearoenix::physics::constraint::TrackerSpringJointSpring>(
//                            new gearoenix::physics::constraint::TrackerSpringJointSpring(
//                                astmgr->create_id(),
//                                std::shared_ptr<gearoenix::physics::body::Rigid>(
//                                    new gearoenix::physics::body::Rigid(astmgr->create_id(), mdl)),
//                                std::shared_ptr<gearoenix::physics::body::Rigid>(
//                                    new gearoenix::physics::body::Rigid(astmgr->create_id(), cam)),
//                                1.0f, gearoenix::math::Vec3(0.0f, -0.5f, 0.4f).normalized(), 1.0f, 2.0f)));
//                    state = State::GAME;
//                });
//                //rndeng->load_scene(1, [this]() -> void {});
//            }
//        }
//        break;
//    }
//    default:
//        break;
//    }
//}
//
GEAROENIX_START(GameApp)
