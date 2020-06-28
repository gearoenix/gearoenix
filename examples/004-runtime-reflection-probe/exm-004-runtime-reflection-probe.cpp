#include "exm-004-runtime-reflection-probe.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/event/cr-ev-engine.hpp>
#include <gearoenix/render/camera/rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/rnd-cmr-manager.hpp>
#include <gearoenix/render/camera/rnd-cmr-perspective.hpp>
#include <gearoenix/render/camera/rnd-cmr-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/render/graph/tree/rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/material/rnd-mat-pbr.hpp>
#include <gearoenix/render/material/rnd-mat-skybox-equirectangular.hpp>
#include <gearoenix/render/mesh/rnd-msh-manager.hpp>
#include <gearoenix/render/mesh/rnd-msh-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/model/rnd-mdl-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-static.hpp>
#include <gearoenix/render/reflection/rnd-rfl-manager.hpp>
#include <gearoenix/render/reflection/rnd-rfl-runtime.hpp>
#include <gearoenix/render/scene/rnd-scn-game.hpp>
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/render/skybox/rnd-sky-equirectangular.hpp>
#include <gearoenix/render/skybox/rnd-sky-manager.hpp>
#include <gearoenix/render/texture/rnd-txt-manager.hpp>
#include <gearoenix/render/texture/rnd-txt-texture-2d.hpp>
#include <gearoenix/system/sys-app.hpp>

template <class T>
using GxEndCaller = gearoenix::core::sync::EndCaller<T>;

using GxEndCallerIgnore = gearoenix::core::sync::EndCallerIgnore;
using GxEndCallerIgnored = GxEndCaller<GxEndCallerIgnore>;
using GxGrTree = gearoenix::render::graph::tree::Tree;
using GxMatPbr = gearoenix::render::material::Pbr;
using GxMdManager = gearoenix::render::model::Manager;
using GxMdMesh = gearoenix::render::model::Mesh;
using GxMesh = gearoenix::render::mesh::Mesh;
using GxPersCam = gearoenix::render::camera::Perspective;
using GxTexture2D = gearoenix::render::texture::Texture2D;
using GxTxtSampleInfo = gearoenix::render::texture::SampleInfo;
using GxTxtFilter = gearoenix::render::texture::Filter;
using GxRtReflect = gearoenix::render::reflection::Runtime;
using GxVec3 = gearoenix::math::Vec3<double>;
using GxStaticModel = gearoenix::render::model::Static;
using GxSkyEqrect = gearoenix::render::skybox::Equirectangular;

Example004RuntimeReflectionProbeApp::Example004RuntimeReflectionProbeApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored end_call([this] {
        scn->set_enability(true);
    });

    GxEndCaller<GxGameScene> scn_call([end_call](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxSkyEqrect> sky_call([end_call](const std::shared_ptr<GxSkyEqrect>&) {});
    GxEndCaller<GxRtReflect> rtr_call([end_call](const std::shared_ptr<GxRtReflect>&) {});
    GxEndCaller<GxStaticModel> mdl_call([end_call](const std::shared_ptr<GxStaticModel>&) {});
    GxEndCaller<GxMesh> msh_call([end_call](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxTexture2D> txt_call([](const std::shared_ptr<GxTexture2D>&) {});

    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const msh_mgr = ast_mgr->get_mesh_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    auto* const sky_mgr = ast_mgr->get_skybox_manager();
    auto* const rfl_mgr = ast_mgr->get_reflection_manager();
    auto* const txt_mgr = ast_mgr->get_texture_manager();

    render_tree = std::make_unique<GxGrPbr>(render_engine, end_call);
    render_engine->set_render_tree(render_tree.get());

    scn = ast_mgr->get_scene_manager()->create<GxGameScene>("scn", scn_call);

    {
        auto cam = ast_mgr->get_camera_manager()->create<GxPersCam>("cam");
        cam->get_transformation()->look_at(GxVec3(0.0, 5.0, 0.0), GxVec3(0.0, 0.0, 0.0), GxVec3(0.0, 0.0, 1.0));
        scn->add_camera(cam);
        cam_ctrl = std::make_shared<GxCamCtrl>(std::move(cam));
    }

    {
        auto sky = sky_mgr->create<GxSkyEqrect>("sky", sky_call);
        GxTxtSampleInfo smp;
        smp.min_filter = GxTxtFilter::Linear;
        sky->get_mat_equ()->set_color(txt_mgr->create_2d_f("sky", "default-equirectangular-sky.hdr", txt_call, smp));
        scn->add_skybox(std::move(sky));
    }

    const auto rtr2 = rfl_mgr->create<GxRtReflect>("rtr2", rtr_call);
    rtr2->set_enabled(false);
    rtr2->set_minimum_receiving_radius(3.0);
    rtr2->set_maximum_receiving_radius(100.0);
    scn->add_reflection(rtr2);

    const auto rtr1 = rfl_mgr->create<GxRtReflect>("rtr1", rtr_call);
    rtr1->set_minimum_receiving_radius(10.0);
    rtr1->set_maximum_receiving_radius(100.0);
    scn->add_reflection(rtr1);

    const auto sphere_mesh = msh_mgr->create_icosphere(msh_call);

    const std::shared_ptr<GxMatPbr> mat1(new GxMatPbr(render_engine, end_call));
    mat1->set_color(0xFF4AA2FF, end_call);
    mat1->set_roughness_factor(0.5f);
    mat1->set_metallic_factor(0.9f);

    const std::shared_ptr<GxMatPbr> mat2(new GxMatPbr(render_engine, end_call));
    mat2->set_color(0x724AFFFF, end_call);
    mat2->set_roughness_factor(0.1f);
    mat2->set_metallic_factor(0.1f);

    const std::shared_ptr<GxMatPbr> mat3(new GxMatPbr(render_engine, end_call));
    mat3->set_color(0xFF4FFFFF, end_call);
    mat3->set_roughness_factor(0.00001f);
    mat3->set_metallic_factor(0.999999f);

    const auto mdl1 = mdl_mgr->create<GxStaticModel>("mdl1", mdl_call);
    mdl1->add_mesh(std::make_shared<GxMdMesh>(sphere_mesh, mat1));
    mdl1->get_transformation()->set_location(GxVec3(4.0, 0.0, 0.0));
    mdl1->set_enabled(false);
    scn->add_model(mdl1);

    const auto mdl2 = mdl_mgr->create<GxStaticModel>("mdl2", mdl_call);
    mdl2->add_mesh(std::make_shared<GxMdMesh>(sphere_mesh, mat2));
    mdl2->get_transformation()->set_location(GxVec3(-4.0, 0.0, 0.0));
    mdl2->set_enabled(false);
    scn->add_model(mdl2);

    const auto mdl3 = mdl_mgr->create<GxStaticModel>("mdl3", mdl_call);
    mdl3->add_mesh(std::make_shared<GxMdMesh>(sphere_mesh, mat3));
    mdl3->get_transformation()->local_scale(2.5);
    mdl3->set_enabled(false);
    scn->add_model(mdl3);

    rtr1->set_on_rendered([rtr1, rtr2, mdl1, mdl2, mdl3] {
        mdl1->set_hooked_reflection(rtr1);
        mdl2->set_hooked_reflection(rtr1);
        mdl3->set_hooked_reflection(rtr1);
        mdl1->set_enabled(true);
        mdl2->set_enabled(true);
        mdl3->set_enabled(true);
        rtr2->set_enabled(true);
        rtr2->set_on_rendered([rtr2, mdl3] {
            mdl3->set_hooked_reflection(rtr2);
        });
    });
}

Example004RuntimeReflectionProbeApp::~Example004RuntimeReflectionProbeApp() noexcept = default;

void Example004RuntimeReflectionProbeApp::update() noexcept
{
}

void Example004RuntimeReflectionProbeApp::terminate() noexcept
{
}

GEAROENIX_START(Example004RuntimeReflectionProbeApp)
