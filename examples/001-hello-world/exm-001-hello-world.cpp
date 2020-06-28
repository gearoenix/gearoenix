#include "exm-001-hello-world.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/physics/body/phs-bd-rigid.hpp>
#include <gearoenix/render/camera/rnd-cmr-manager.hpp>
#include <gearoenix/render/camera/rnd-cmr-perspective.hpp>
#include <gearoenix/render/camera/rnd-cmr-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/render/graph/tree/rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/light/rnd-lt-directional.hpp>
#include <gearoenix/render/light/rnd-lt-manager.hpp>
#include <gearoenix/render/material/rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/rnd-msh-manager.hpp>
#include <gearoenix/render/mesh/rnd-msh-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/model/rnd-mdl-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-static.hpp>
#include <gearoenix/render/scene/rnd-scn-game.hpp>
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <memory>

template <class T>
using GxEndCaller = gearoenix::core::sync::EndCaller<T>;

using GxEndCallerIgnore = gearoenix::core::sync::EndCallerIgnore;
using GxEndCallerIgnored = GxEndCaller<GxEndCallerIgnore>;
using GxGrTree = gearoenix::render::graph::tree::Tree;
using GxMaterial = gearoenix::render::material::Pbr;
using GxMdManager = gearoenix::render::model::Manager;
using GxMdMesh = gearoenix::render::model::Mesh;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxStaticModel = gearoenix::render::model::Static;
using GxVec3 = gearoenix::math::Vec3<double>;
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;

GameApp::GameApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored end_call([this] { scn->set_enability(true); });
    GxEndCaller<GxGameScene> scn_call([end_call](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxMesh> msh_call([end_call](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxStaticModel> mdl_call([end_call](const std::shared_ptr<GxStaticModel>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, end_call);
    render_engine->set_render_tree(render_tree.get());
    gearoenix::core::asset::Manager* const ast_mgr = sys_app->get_asset_manager();
    scn = ast_mgr->get_scene_manager()->create<GxGameScene>("scene", scn_call);

    std::shared_ptr<GxPersCam> cam = ast_mgr->get_camera_manager()->create<GxPersCam>("cam-pers");
    cam->set_cascaded_shadow_enabled(true);
    cam_trn = dynamic_cast<GxCamTran*>(cam->get_transformation());
    cam_trn->look_at(GxVec3(20.0f, 20.0f, 10.0f), GxVec3(0.0f, 0.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f));
    scn->add_camera(cam);

    const std::shared_ptr<GxDirLight> light = ast_mgr->get_light_manager()->create<GxDirLight>("shadow-light");
    light->enable_shadowing();
    light->set_direction(GxVec3(-1.0f, 0.0f, -2.0f).normalized());
    light->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light);
    const std::shared_ptr<GxDirLight> light1 = ast_mgr->get_light_manager()->create<GxDirLight>("l1");
    light1->set_direction(GxVec3(1.0f, 0.0f, -1.0f).normalized());
    light1->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light1);
    const std::shared_ptr<GxDirLight> light2 = ast_mgr->get_light_manager()->create<GxDirLight>("l2");
    light2->set_direction(GxVec3(-1.0f, 0.0f, -1.0f).normalized());
    light2->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light2);
    const std::shared_ptr<GxDirLight> light3 = ast_mgr->get_light_manager()->create<GxDirLight>("l3");
    light3->set_direction(GxVec3(0.0f, 1.0f, -1.0f).normalized());
    light3->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light3);
    const std::shared_ptr<GxDirLight> light4 = ast_mgr->get_light_manager()->create<GxDirLight>("l4");
    light4->set_direction(GxVec3(0.0f, -1.0f, -1.0f).normalized());
    light4->set_color(GxVec3(0.5f, 0.5f, 0.5f));
    scn->add_light(light4);

    const std::shared_ptr<GxMesh> msh = ast_mgr->get_mesh_manager()->create_icosphere(msh_call);
    const std::shared_ptr<GxMesh> plate_mesh = ast_mgr->get_mesh_manager()->create_plate(msh_call);
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    {
        const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, end_call));
        mat->set_roughness_factor(0.5f);
        mat->set_metallic_factor(0.8f);
        mat->set_color(0.0f, 0.999f, 0.0f, end_call);
        const std::shared_ptr<GxStaticModel> mdl = mdl_mgr->create<GxStaticModel>("ground", mdl_call);
        mdl->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, mat));
        auto* trans = mdl->get_transformation();
        trans->set_location(GxVec3(0.0f, 0.0f, -5.0f));
        trans->local_scale(14.0f);
        scn->add_model(mdl);
    }
    for (float y = -10.0f, roughness = 0.1f; y < 10.1f; y += 2.5f, roughness += 0.1f) {
        for (float x = -10.0f, metallic = 0.1f; x < 10.1f; x += 2.5f, metallic += 0.1f) {
            const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, end_call));
            mat->set_roughness_factor(roughness);
            mat->set_metallic_factor(metallic);
            const auto mdl = mdl_mgr->create<GxStaticModel>(
                "model-" + std::to_string(metallic) + "-" + std::to_string(roughness),
                mdl_call);
            mdl->add_mesh(std::make_shared<GxMdMesh>(msh, mat));
            mdl->get_transformation()->set_location(GxVec3(x, y, 0.0f));
            scn->add_model(mdl);
        }
    }
}

void GameApp::update() noexcept
{
    cam_trn->global_rotate(render_engine->get_delta_time() * 0.1f, GxVec3(0.0f, 0.0f, 1.0f), GxVec3(0.0f, 0.0f, 0.0f));
}

void GameApp::terminate() noexcept
{
    gearoenix::core::Application::terminate();
    scn = nullptr;
    cam_trn = nullptr;
    render_tree = nullptr;
}

GEAROENIX_START(GameApp)
