#include "exm-002-asset-loading.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
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

GameApp::GameApp(gearoenix::system::Application * const sys_app) noexcept
	: gearoenix::core::Application::Application(sys_app)
{
	const GxEndCallerIgnored endcall([this] { scn->enable(); });
	GxEndCaller<GxScene> scncall([this, endcall](std::shared_ptr<GxScene> s) {
		scn = std::move(s);
	});

	render_engine->set_render_tree(new GxGrPbr(render_engine, endcall));

	const auto& astmgr = sys_app->get_asset_manager();
	const auto& scnmgr = astmgr->get_scene_manager();
	
	scnmgr->get_gx3d(1024, scncall);
}

GameApp::~GameApp() noexcept
{}

void GameApp::update() noexcept {}

void GameApp::terminate() noexcept {
	gearoenix::core::Application::terminate();
	scn = nullptr;
}

GEAROENIX_START(GameApp)