#include "exm-003-net-hello-world.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/event/cr-ev-sys-system.hpp>
#include <gearoenix/core/event/cr-ev-ui-ui.hpp>
#include <gearoenix/core/event/cr-ev-engine.hpp>
#include <gearoenix/physics/body/phs-bd-rigid.hpp>
#include <gearoenix/physics/collider/phs-cld-sphere.hpp>
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
#include <gearoenix/render/scene/rnd-scn-game.hpp>
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
#include <gearoenix/render/widget/rnd-wdg-modal.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <gearoenix/system/sys-log.hpp>
#include <random>

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
using GxReal = gearoenix::core::Real;
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;
using GxCldSphere = gearoenix::physics::collider::Sphere;

struct ShelfInfo {
	std::uniform_real_distribution<GxReal> rand_genx;
	std::uniform_real_distribution<GxReal> rand_genz;
	GxReal y;
};

GameApp::GameApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
	std::random_device rand_dev;
	std::default_random_engine rand_eng(rand_dev());
	std::uniform_real_distribution<GxReal> rand_gen1(0.2f, 0.4f);
	std::uniform_real_distribution<GxReal> rand_gen2(0.001f, 0.999f);
	std::uniform_int_distribution<int> rand_gen3(10, 15);

	const GxReal marginx = 0.02f;
	const GxReal marginz = 0.01f;

	ShelfInfo shelves_info[] = {
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			9.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			9.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			9.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			9.0f,
		},
		/////////////////////////////////////////////////////////////////////////////
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			7.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			7.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			7.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			7.0f,
		},
		/////////////////////////////////////////////////////////////////////////////
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			1.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			1.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			1.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			1.0f,
		},
		/////////////////////////////////////////////////////////////////////////////
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			-1.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			-1.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			-1.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			-1.0f,
		},
		/////////////////////////////////////////////////////////////////////////////
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			-7.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			-7.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			-7.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			-7.0f,
		},
		/////////////////////////////////////////////////////////////////////////////
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			-9.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx - 13.0f, -2.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			-9.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz, 2.0f - marginz),
			-9.0f,
		},
		{
			std::uniform_real_distribution<GxReal>(marginx + 2.0f, 13.0f - marginx),
			std::uniform_real_distribution<GxReal>(marginz - 3.0f, -1.0f - marginz),
			-9.0f,
		},
	};

    const GxEndCallerIgnored endcall([this] { 
		scn->set_enability(true);
        uiscn->set_enability(true);
		system_application->get_event_engine()->add_listner(gearoenix::core::event::Id::ButtonMouse, 1.0f, this);
	});
    GxEndCaller<GxGameScene> scncall([endcall](std::shared_ptr<GxGameScene>) {});
    GxEndCaller<GxUiScene> uiscncall([endcall](std::shared_ptr<GxUiScene>) {});
    GxEndCaller<GxMesh> mshcall([endcall](std::shared_ptr<GxMesh>) {});
    GxEndCaller<GxModel> mdlcall([endcall](std::shared_ptr<GxModel>) {});
    GxEndCaller<GxModal> mdacall([endcall](std::shared_ptr<GxModal>) {});
    /// TODO: keep the render tree pointer and delete it later
    render_engine->set_render_tree(new GxGrPbr(render_engine, endcall));
    gearoenix::core::asset::Manager* const astmgr = sys_app->get_asset_manager();
    scn = astmgr->get_scene_manager()->create<GxGameScene>(scncall);
    uiscn = astmgr->get_scene_manager()->create<GxUiScene>(uiscncall);

    cam = astmgr->get_camera_manager()->create<GxPersCam>();
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
    const std::shared_ptr<GxMesh> plate_mesh = astmgr->get_mesh_manager()->create_plate(mshcall);
    const std::shared_ptr<GxMdManager> &mdlmgr = astmgr->get_model_manager();
    {
        const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
        mat->set_roughness_factor(0.5f);
        mat->set_metallic_factor(0.8f);
        mat->set_color(0.0f, 0.999f, 0.0f, endcall);
        const std::shared_ptr<GxModel> mdl = mdlmgr->create<GxModel>(mdlcall);
        mdl->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, mat));
        auto *trans = mdl->get_transformation();
        trans->set_location(GxVec3(0.0f, 0.0f, -5.0f));
        trans->local_scale(14.0f);
        scn->add_model(mdl);
    }
	for (auto& s : shelves_info)
	{
		const int items_count = rand_gen3(rand_eng);
		for (int i = 0; i < items_count; ++i) 
		{
			const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
			mat->set_roughness_factor(rand_gen2(rand_eng));
			mat->set_metallic_factor(rand_gen2(rand_eng));
			mat->set_color(rand_gen2(rand_eng), rand_gen2(rand_eng), rand_gen2(rand_eng), endcall);
			const std::shared_ptr<GxModel> mdl = mdlmgr->create<GxModel>(mdlcall);
			mdl->add_mesh(std::make_shared<GxMdMesh>(msh, mat));
			auto* tran = mdl->get_transformation();
			const GxVec3 position(s.rand_genx(rand_eng), s.y, s.rand_genz(rand_eng));
			const auto scale = rand_gen1(rand_eng);
			tran->set_location(position);
			tran->local_scale(scale);
			mdl->set_collider(std::make_unique<GxCldSphere>(position, scale));
			scn->add_model(mdl);
		}
	}

    modal = mdlmgr->create<GxModal>(mdacall);
    modal->set_enability(false);
    uiscn->add_model(modal);
}

void GameApp::update() noexcept
{
    camtrn->global_rotate(render_engine->get_delta_time() * 0.1f, GxVec3(0.0f, 0.0f, 1.0f));
}

void GameApp::terminate() noexcept
{
    gearoenix::core::Application::terminate();
    scn = nullptr;
    camtrn = nullptr;
}

bool GameApp::on_event(const gearoenix::core::event::Data& event_data) noexcept
{
	switch (event_data.source)
	{
	case gearoenix::core::event::Id::ButtonMouse:
	{
		const auto d = std::get<gearoenix::core::event::button::Data>(event_data.data);
		if (d.key == gearoenix::core::event::button::KeyId::Left && d.action == gearoenix::core::event::button::ActionId::Press) {
			const auto ray = cam->create_ray3(d.x, d.y);
			auto hit = scn->hit(ray, std::numeric_limits<gearoenix::core::Real>::max());
			if (hit.has_value()) {
				auto* cld = hit.value().second;
				auto* mdl = cld->get_parent();
                static float cx = 0.0f, cy = 0.0f, cz = 0.0f; cx = 1.0f - cx; cy = 1.0f - cy; cz = 1.0f - cz;
                mdl->get_meshes().begin()->second->get_material()->set_color(cx, cy, cz, GxEndCallerIgnored([] {}));
                modal->set_enability(true);
			}
		}
			
		break;
	}
	default:
		break;
	}
	return false;
}

GEAROENIX_START(GameApp)
