#include "gx-tool-ibl-main.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/cr-string.hpp>
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
#include <gearoenix/render/texture/rnd-txt-texture-cube.hpp>
#include <gearoenix/render/widget/rnd-wdg-button.hpp>
#include <gearoenix/render/widget/rnd-wdg-edit.hpp>
#include <gearoenix/render/widget/rnd-wdg-modal.hpp>
#include <gearoenix/render/widget/rnd-wdg-text.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <gearoenix/system/sys-args.hpp>

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
using GxMovementBase = gearoenix::core::event::movement::Base2D;
using GxTxtSampleInfo = gearoenix::render::texture::SampleInfo;
using GxTxtFilter = gearoenix::render::texture::Filter;

void IblBaker::on_open() noexcept
{
    const auto& f = file_location->get_text();
    if (f.empty())
        return;
    environment_file = gearoenix::core::String::to_string(f);
    open_environment();
}

void IblBaker::argument_handling() noexcept
{
    GXLOGD("Going to parsing arguments.")
    const auto* const args = system_application->get_arguments();
    args->get_value("environment-file", environment_file);
    args->get_value("baked-cube-file", baked_cube_file);
    args->get_value("baked-cube-resolution", baked_cube_resolution);
    args->get_value("irradiance-file", irradiance_file);
    args->get_value("irradiance-resolution", irradiance_resolution);
    args->get_value("radiance-file", radiance_file);
    args->get_value("radiance-resolution", radiance_resolution);
    auto& conf = system_application->get_configuration().render_config;
    conf.set_runtime_reflection_environment_resolution(baked_cube_resolution);
    conf.set_runtime_reflection_irradiance_resolution(irradiance_resolution);
    conf.set_runtime_reflection_radiance_resolution(radiance_resolution);
    GXLOGD("Arguments parsed.")
}

IblBaker::IblBaker(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
    , called_from_cli(system_application->get_arguments()->get_has_tokens())
{
    GXLOGD("IBL baker started.")
    if (called_from_cli)
        argument_handling();

    const GxEndCallerIgnored end_call([this] {
        uiscn->set_enability(true);
        scn->set_enability(true);
        obj_scn->set_enability(true);
        auto* const event_engine = system_application->get_event_engine();
        event_engine->add_listener(GxEventId::ButtonMouse, 0.0f, this);
        event_engine->add_listener(GxEventId::MovementMouse, 0.0f, this);
    });

    GxEndCaller<GxUiScene> ui_scn_call([end_call](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxGameScene> scn_call([end_call](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxTextWdg> txw_call([end_call](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxEditWdg> edt_call([end_call](const std::shared_ptr<GxEditWdg>&) {});
    GxEndCaller<GxButton> btn_call([end_call](const std::shared_ptr<GxButton>&) {});
    GxEndCaller<GxSkyEqrect> sky_call([end_call](const std::shared_ptr<GxSkyEqrect>&) {});
    GxEndCaller<GxRtReflect> rtr_call([end_call](const std::shared_ptr<GxRtReflect>&) {});
    GxEndCaller<GxStaticModel> mdl_call([end_call](const std::shared_ptr<GxStaticModel>&) {});
    GxEndCaller<GxMesh> msh_call([end_call](const std::shared_ptr<GxMesh>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, end_call);
    render_engine->set_render_tree(render_tree.get());

    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const msh_mgr = ast_mgr->get_mesh_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    auto* const sky_mgr = ast_mgr->get_skybox_manager();
    auto* const rfl_mgr = ast_mgr->get_reflection_manager();
    auto* const txt_mgr = ast_mgr->get_texture_manager();

    uiscn = ast_mgr->get_scene_manager()->create<GxUiScene>(ui_scn_call);
    scn = ast_mgr->get_scene_manager()->create<GxGameScene>(scn_call);

    cam = ast_mgr->get_camera_manager()->create<GxPersCam>();
    cam_trn = dynamic_cast<GxCamTran*>(cam->get_transformation());
    cam_trn->look_at(GxVec3(25.0, 0.0, 0.0), GxVec3(0.0, 0.0, 0.0), GxVec3(0.0, 0.0, 1.0));
    scn->add_camera(cam);

    sky = sky_mgr->create<GxSkyEqrect>(sky_call);
    scn->add_skybox(sky);

    rtr = rfl_mgr->create<GxRtReflect>(rtr_call);
    scn->add_reflection(rtr);

    auto tmp_txt = mdl_mgr->create<GxTextWdg>(txw_call);
    auto* tmp_tran = tmp_txt->get_transformation();
    tmp_tran->local_scale(0.04f);
    tmp_tran->set_location(GxVec3(0.0, 0.85, 0.1));
    tmp_txt->set_text(L"IBL baker for Gearoenix game engine", end_call);
    uiscn->add_model(tmp_txt);

    tmp_txt = mdl_mgr->create<GxTextWdg>(txw_call);
    tmp_tran = tmp_txt->get_transformation();
    tmp_tran->local_scale(0.03f);
    tmp_tran->set_location(GxVec3(-0.75, 0.75, 0.1));
    tmp_txt->set_text(L"HDR file location:", end_call);
    uiscn->add_model(tmp_txt);

    auto open_button = mdl_mgr->create<GxButton>(btn_call);
    tmp_tran = open_button->get_transformation();
    tmp_tran->local_scale(0.04f);
    tmp_tran->local_x_scale(4.0f);
    tmp_tran->set_location(GxVec3(0.75, 0.75, 0.1));
    open_button->set_text(L"Open File", end_call);
    if (!called_from_cli)
        open_button->set_on_click([this] { on_open(); });
    uiscn->add_model(open_button);

    file_location = mdl_mgr->create<GxEditWdg>(edt_call);
    tmp_tran = file_location->get_transformation();
    tmp_tran->local_scale(0.04);
    tmp_tran->local_x_scale(14.0);
    tmp_tran->set_location(GxVec3(0.025, 0.75, 0.1));
    file_location->set_hint_text(L"<Fill it with 'file location'>", end_call);
    uiscn->add_model(file_location);

    obj_scn = ast_mgr->get_scene_manager()->create<GxGameScene>(scn_call);
    obj_scn->set_layer((scn->get_layer() + uiscn->get_layer()) * 0.5);

    obj_cam = ast_mgr->get_camera_manager()->create<GxPersCam>();
    obj_cam_trn = dynamic_cast<GxCamTran*>(obj_cam->get_transformation());
    obj_cam_trn->look_at(GxVec3(25.0, 0.0, 0.0), GxVec3(0.0, 0.0, 0.0), GxVec3(0.0, 0.0, 1.0));
    obj_scn->add_camera(obj_cam);

    //    {
    //        const auto cube_mesh = msh_mgr->create_cube(msh_call);
    //        const std::shared_ptr<GxMatPbr> mat(new GxMatPbr(render_engine, end_call));
    //        mat->set_roughness_factor(0.5f);
    //        mat->set_metallic_factor(0.5f);
    //        const auto mdl = mdl_mgr->create<GxStaticModel>(mdl_call);
    //        mdl->add_mesh(std::make_shared<GxMdMesh>(cube_mesh, mat));
    //        mdl->get_transformation()->local_scale(10.0);
    //        obj_scn->add_model(mdl);
    //        mdl->set_hooked_reflection(rtr);
    //    }

    //        {
    //            const auto sphere_mesh = msh_mgr->create_icosphere(msh_call);
    //            const std::shared_ptr<GxMatPbr> mat(new GxMatPbr(render_engine, end_call));
    //            mat->set_roughness_factor(0.999f);
    //            mat->set_metallic_factor(0.5f);
    //            const auto mdl = mdl_mgr->create<GxStaticModel>(mdl_call);
    //            mdl->add_mesh(std::make_shared<GxMdMesh>(sphere_mesh, mat));
    //            mdl->get_transformation()->local_scale(10.0);
    //            obj_scn->add_model(mdl);
    //            mdl->set_hooked_reflection(rtr);
    //        }

    const auto obj_msh = msh_mgr->create_icosphere(msh_call);

    for (float y = -10.0f, roughness = 0.1f; y < 10.1f; y += 2.5f, roughness += 0.1f) {
        for (float x = -10.0f, metallic = 0.1f; x < 10.1f; x += 2.5f, metallic += 0.1f) {
            const std::shared_ptr<GxMatPbr> mat(new GxMatPbr(render_engine, end_call));
            mat->set_roughness_factor(roughness);
            mat->set_metallic_factor(metallic);
            mat->set_color(1.0f, 0.0f, 0.0f, end_call);
            const auto mdl = mdl_mgr->create<GxStaticModel>(mdl_call);
            mdl->add_mesh(std::make_shared<GxMdMesh>(obj_msh, mat));
            mdl->get_transformation()->set_location(GxVec3(x, y, 0.0f));
            obj_scn->add_model(mdl);
            mdl->set_hooked_reflection(rtr);
        }
    }

    if (called_from_cli) {
        GxEndCaller<GxTexture2D> txt_call([end_call](const std::shared_ptr<GxTexture2D>&) {});
        GxTxtSampleInfo smp;
        smp.min_filter = GxTxtFilter::Linear;
        sky->get_mat_equ()->set_color(txt_mgr->create_2d_f(environment_file, txt_call, smp));

        rtr->set_on_rendered([this]() {
            GXLOGD("Runtime reflection rendered.")

            GxEndCallerIgnored termination_call([this] {
                system_application->quit();
            });

            static_cast<gearoenix::render::texture::Texture*>(rtr->get_environment().get())->write_gx3d(baked_cube_file, termination_call);
            static_cast<gearoenix::render::texture::Texture*>(rtr->get_radiance().get())->write_gx3d(radiance_file, termination_call);
            static_cast<gearoenix::render::texture::Texture*>(rtr->get_irradiance().get())->write_gx3d(irradiance_file, termination_call);
        });
    }
    GXLOGD("Initialization ended.")
}

IblBaker::~IblBaker() noexcept
{
    terminate();
}

void IblBaker::update() noexcept
{
}

void IblBaker::terminate() noexcept
{
}

bool IblBaker::on_event(const gearoenix::core::event::Data& d) noexcept
{
    switch (d.get_source()) {
    case GxEventId::ButtonMouse: {
        const auto& data = std::get<GxMouseData>(d.get_data());
        if (data.get_key() == GxMouseKeyId::Left)
            camera_rotation_enabled = data.get_action() == GxMouseActionId::Press;
        break;
    }
    case GxEventId::MovementMouse: {
        if (camera_rotation_enabled) {
            const auto& data = std::get<gearoenix::core::event::movement::Base2D>(d.get_data());
            const auto rot_x = data.get_point().get_delta_previous_position().y;
            const auto rot_z = data.get_point().get_delta_previous_position().x;
            cam_trn->local_rotate(rot_z, GxVec3(0.0, 0.0, 1.0));
            cam_trn->local_x_rotate(rot_x);
            obj_cam_trn->global_rotate(rot_z, GxVec3(0.0, 0.0, 1.0), GxVec3(0.0, 0.0, 0.0));
            obj_cam_trn->global_rotate(rot_x, obj_cam_trn->get_x_axis(), GxVec3(0.0, 0.0, 0.0));
        }
        break;
    }
    default:
        GXUNEXPECTED
    }
    return false;
}

void IblBaker::open_environment() noexcept
{
    auto* const ast_mgr = system_application->get_asset_manager();
    auto* const txt_mgr = ast_mgr->get_texture_manager();
    GxEndCaller<GxTexture2D> txt_call([this](const std::shared_ptr<GxTexture2D>& t) {
        sky->get_mat_equ()->set_color(t);
        rtr->set_pending_to_start(true);
        rtr->set_on_rendered([this]() {
            static_cast<gearoenix::render::texture::Texture*>(rtr->get_radiance().get())->write_gx3d(radiance_file, GX_DEFAULT_IGNORED_END_CALLER);
            static_cast<gearoenix::render::texture::Texture*>(rtr->get_irradiance().get())->write_gx3d(irradiance_file, GX_DEFAULT_IGNORED_END_CALLER);
        });
    });
    GxTxtSampleInfo smp;
    smp.min_filter = GxTxtFilter::Linear;
    /// TODO failure management
    (void)txt_mgr->create_2d_f(environment_file, txt_call, smp);
}

GEAROENIX_START(IblBaker)
