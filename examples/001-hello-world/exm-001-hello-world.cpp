#include "exm-001-hello-world.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/physics/body/phs-bd-rigid.hpp>
#include <gearoenix/physics/constraint/phs-cns-tracker-spring-joint-spring.hpp>
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
#include <gearoenix/render/model/rnd-mdl-static.hpp>
#include <gearoenix/render/scene/rnd-scn-game.hpp>
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/system/sys-app.hpp>

template <class T>
using GxEndCaller = gearoenix::core::sync::EndCaller<T>;

using GxEndCallerIgnore = gearoenix::core::sync::EndCallerIgnore;
using GxEndCallerIgnored = GxEndCaller<GxEndCallerIgnore>;
using GxGrTree = gearoenix::render::graph::tree::Tree;
using GxMaterial = gearoenix::render::material::Material;
using GxMdManager = gearoenix::render::model::Manager;
using GxMdMesh = gearoenix::render::model::Mesh;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxStaticModel = gearoenix::render::model::Static;
using GxVec3 = gearoenix::math::Vec3;
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;

GameApp::GameApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored endcall([this] { scn->set_enability(true); });
    GxEndCaller<GxGameScene> scncall([endcall](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxMesh> mshcall([endcall](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxStaticModel> mdlcall([endcall](const std::shared_ptr<GxStaticModel>&) {});

    render_tree = std::unique_ptr<GxGrPbr>(new GxGrPbr(render_engine, endcall));
    render_engine->set_render_tree(render_tree.get());
    gearoenix::core::asset::Manager* const astmgr = sys_app->get_asset_manager();
    scn = astmgr->get_scene_manager()->create<GxGameScene>(scncall);

    std::shared_ptr<GxPersCam> cam = astmgr->get_camera_manager()->create<GxPersCam>();
    camtrn = dynamic_cast<GxCamTran*>(cam->get_transformation());
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
    const std::shared_ptr<GxMesh> plate_mesh = astmgr->get_mesh_manager()->create_plate(mshcall);
    auto* const mdlmgr = astmgr->get_model_manager();
    {
        const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
        mat->set_roughness_factor(0.5f);
        mat->set_metallic_factor(0.8f);
        mat->set_color(0.0f, 0.999f, 0.0f, endcall);
        const std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
        mdl->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, mat));
        auto* trans = mdl->get_transformation();
        trans->set_location(GxVec3(0.0f, 0.0f, -5.0f));
        trans->local_scale(14.0f);
        scn->add_model(mdl);
    }
    for (gearoenix::core::Real y = -10.0f, roughness = 0.1f; y < 10.1f; y += 2.5f, roughness += 0.1f) {
        for (gearoenix::core::Real x = -10.0f, metallic = 0.1f; x < 10.1f; x += 2.5f, metallic += 0.1f) {
            const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
            mat->set_roughness_factor(roughness);
            mat->set_metallic_factor(metallic);
            const auto mdl = mdlmgr->create<GxStaticModel>(mdlcall);
            mdl->add_mesh(std::make_shared<GxMdMesh>(msh, mat));
            mdl->get_transformation()->set_location(GxVec3(x, y, 0.0f));
            scn->add_model(mdl);
        }
    }
}

void GameApp::update() noexcept
{
    camtrn->global_rotate(render_engine->get_delta_time() * 0.1f, GxVec3(0.0f, 0.0f, 1.0f), GxVec3(0.0f, 0.0f, 0.0f));
}

void GameApp::terminate() noexcept
{
    gearoenix::core::Application::terminate();
    scn = nullptr;
    camtrn = nullptr;
    render_tree = nullptr;
}

GEAROENIX_START(GameApp)
