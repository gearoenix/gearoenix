#include "exm-004-runtime-reflection-probe.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/event/cr-ev-engine.hpp>
#include <gearoenix/math/math-aabb.hpp>
#include <gearoenix/math/math-vertex.hpp>
#include <gearoenix/physics/body/phs-bd-rigid.hpp>
#include <gearoenix/physics/collider/phs-cld-sphere.hpp>
#include <gearoenix/physics/constraint/phs-cns-tracker-spring-joint-spring.hpp>
#include <gearoenix/render/camera/rnd-cmr-manager.hpp>
#include <gearoenix/render/camera/rnd-cmr-perspective.hpp>
#include <gearoenix/render/camera/rnd-cmr-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/render/graph/tree/rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/light/rnd-lt-directional.hpp>
#include <gearoenix/render/light/rnd-lt-manager.hpp>
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
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
#include <gearoenix/render/skybox/rnd-sky-equirectangular.hpp>
#include <gearoenix/render/skybox/rnd-sky-manager.hpp>
#include <gearoenix/render/texture/rnd-txt-manager.hpp>
#include <gearoenix/render/texture/rnd-txt-texture-2d.hpp>
#include <gearoenix/render/widget/rnd-wdg-button.hpp>
#include <gearoenix/render/widget/rnd-wdg-edit.hpp>
#include <gearoenix/render/widget/rnd-wdg-modal.hpp>
#include <gearoenix/render/widget/rnd-wdg-text.hpp>
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
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;
using GxTexture = gearoenix::render::texture::Texture;
using GxTexture2D = gearoenix::render::texture::Texture2D;
using GxCldSphere = gearoenix::physics::collider::Sphere;
using GxVertex = gearoenix::math::BasicVertex;
using GxAabb3 = gearoenix::math::Aabb3;
using GxStream = gearoenix::system::stream::Stream;
using GxEventId = gearoenix::core::event::Id;
using GxMouseData = gearoenix::core::event::button::MouseData;
using GxMouseKeyId = gearoenix::core::event::button::MouseKeyId;
using GxMouseActionId = gearoenix::core::event::button::MouseActionId;
using GxMovementBase = gearoenix::core::event::movement::Base;
using GxTxtSampleInfo = gearoenix::render::texture::SampleInfo;
using GxTxtFilter = gearoenix::render::texture::Filter;
using GxRtReflect = gearoenix::render::reflection::Runtime;

Example004RuntimeReflectionProbeApp::Example004RuntimeReflectionProbeApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored end_call([this] {
        scn->set_enability(true);
        auto* const event_engine = system_application->get_event_engine();
        event_engine->add_listener(GxEventId::ButtonMouse, 0.0f, this);
        event_engine->add_listener(GxEventId::MovementMouse, 0.0f, this);
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

    scn = ast_mgr->get_scene_manager()->create<GxGameScene>(scn_call);

    cam = ast_mgr->get_camera_manager()->create<GxPersCam>();
    cam_trn = dynamic_cast<GxCamTran*>(cam->get_transformation());
    cam_trn->look_at(GxVec3(0.0, 5.0, 0.0), GxVec3(0.0, 0.0, 0.0), GxVec3(0.0, 0.0, 1.0));
    scn->add_camera(cam);

    sky = sky_mgr->create<GxSkyEqrect>(sky_call);
    GxTxtSampleInfo smp;
    smp.min_filter = GxTxtFilter::Linear;
    sky->get_mat_equ()->set_color(txt_mgr->create_2d_f(L"default-equirectangular-sky.hdr", txt_call, smp));
    scn->add_skybox(sky);

    const auto rtr1 = rfl_mgr->create<GxRtReflect>(rtr_call);
    rtr1->set_location(GxVec3(1.75, 0.0, 0.0));
    scn->add_reflection(rtr1);

    const auto rtr2 = rfl_mgr->create<GxRtReflect>(rtr_call);
    rtr2->set_location(GxVec3(-1.75, 0.0, 0.0));
    scn->add_reflection(rtr2);

    const auto cube_mesh = msh_mgr->create_cube(msh_call);

    const std::shared_ptr<GxMatPbr> mat1(new GxMatPbr(render_engine, end_call));
    mat1->set_color(0xFF4AA2FF, end_call);
    mat1->set_roughness_factor(0.0001f);
    mat1->set_metallic_factor(0.9999f);

    const std::shared_ptr<GxMatPbr> mat2(new GxMatPbr(render_engine, end_call));
    mat2->set_color(0x724AFFFF, end_call);
    mat2->set_roughness_factor(0.0001f);
    mat2->set_metallic_factor(0.9999f);

    const auto mdl1 = mdl_mgr->create<GxStaticModel>(mdl_call);
    mdl1->add_mesh(std::make_shared<GxMdMesh>(cube_mesh, mat1));
    mdl1->get_transformation()->set_location(GxVec3(1.75, 0.0, 0.0));
    mdl1->set_hooked_reflection(rtr1);
    scn->add_model(mdl1);

    const auto mdl2 = mdl_mgr->create<GxStaticModel>(mdl_call);
    mdl2->add_mesh(std::make_shared<GxMdMesh>(cube_mesh, mat2));
    mdl2->get_transformation()->set_location(GxVec3(-1.75, 0.0, 0.0));
    mdl2->set_hooked_reflection(rtr2);
    scn->add_model(mdl2);
}

Example004RuntimeReflectionProbeApp::~Example004RuntimeReflectionProbeApp() noexcept
{
    terminate();
}

void Example004RuntimeReflectionProbeApp::update() noexcept
{
}

void Example004RuntimeReflectionProbeApp::terminate() noexcept
{
}

bool Example004RuntimeReflectionProbeApp::on_event(const gearoenix::core::event::Data& d) noexcept
{
    switch (d.source) {
    case GxEventId::ButtonMouse: {
        const auto& data = std::get<GxMouseData>(d.data);
        if (data.key == GxMouseKeyId::Left)
            camera_rotation_enabled = data.action == GxMouseActionId::Press;
        break;
    }
    case GxEventId::MovementMouse: {
        if (camera_rotation_enabled) {
            const auto& data = std::get<gearoenix::core::event::movement::Base>(d.data);
            const auto rot_x = data.delta_position[1];
            const auto rot_z = data.delta_position[0];
            cam_trn->global_rotate(rot_z, GxVec3(0.0, 0.0, 1.0), GxVec3(0.0, 0.0, 0.0));
            cam_trn->global_rotate(rot_x, cam_trn->get_x_axis(), GxVec3(0.0, 0.0, 0.0));
        }
        break;
    }
    default:
        GXUNEXPECTED
    }
    return false;
}

GEAROENIX_START(Example004RuntimeReflectionProbeApp)
