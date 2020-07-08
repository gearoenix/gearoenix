#include "exm-002-asset-loading.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/physics/body/gx-phs-bd-rigid.hpp>
#include <gearoenix/render/camera/rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/rnd-cmr-manager.hpp>
#include <gearoenix/render/camera/rnd-cmr-perspective.hpp>
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
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/render/scene/rnd-scn-scene.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <memory>

template <class T>
using GxEndCaller = gearoenix::core::sync::EndCaller<T>;

using GxEndCallerIgnore = gearoenix::core::sync::EndCallerIgnore;
using GxEndCallerIgnored = GxEndCaller<GxEndCallerIgnore>;
using GxGrTree = gearoenix::render::graph::tree::Tree;
using GxMaterial = gearoenix::render::material::Material;
using GxMdManager = gearoenix::render::model::Manager;
using GxMdMesh = gearoenix::render::model::Mesh;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxModel = gearoenix::render::model::Model;
using GxVec3 = gearoenix::math::Vec3<double>;
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;

GameApp::GameApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored end_call([this] { scn->set_enabled(true); });
    GxEndCaller<GxScene> scn_call([this, end_call](std::shared_ptr<GxScene> s) {
        scn = std::move(s);
        cam_ctrl = std::make_shared<GxCamCtrl>(scn->get_cameras().begin()->second);
    });

    tree = std::make_unique<GxGrPbr>(render_engine, end_call);
    render_engine->set_render_tree(tree.get());

    const auto& ast_mgr = sys_app->get_asset_manager();
    const auto& scn_mgr = ast_mgr->get_scene_manager();

    scn_mgr->get_gx3d(1024, scn_call);
}

GameApp::~GameApp() noexcept = default;

void GameApp::update() noexcept { }

void GameApp::terminate() noexcept
{
    gearoenix::core::Application::terminate();
    scn = nullptr;
}

GEAROENIX_START(GameApp)