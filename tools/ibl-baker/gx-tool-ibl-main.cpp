#include "gx-tool-ibl-main.hpp"
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
#include <gearoenix/render/material/rnd-mat-material.hpp>
#include <gearoenix/render/material/rnd-mat-sky-equirectangular.hpp>
#include <gearoenix/render/mesh/rnd-msh-manager.hpp>
#include <gearoenix/render/mesh/rnd-msh-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/model/rnd-mdl-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-static.hpp>
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
#include <gearoenix/system/stream/sys-stm-stream.hpp>
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
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;
using GxTexture = gearoenix::render::texture::Texture;
using GxTexture2D = gearoenix::render::texture::Texture2D;
using GxCldSphere = gearoenix::physics::collider::Sphere;
using GxVertex = gearoenix::math::BasicVertex;
using GxAabb3 = gearoenix::math::Aabb3;
using GxStream = gearoenix::system::stream::Stream;
using GxSkyEqrect = gearoenix::render::skybox::Equirectangular;

void IblBakerApp::on_open() noexcept
{
    auto const file_content = GxStream::get_file_content(file_location->get_text());
    auto* const ast_mgr = system_application->get_asset_manager();
    auto* const txt_mgr = ast_mgr->get_texture_manager();
    auto* const sky_mgr = ast_mgr->get_skybox_manager();
    GxEndCaller<GxSkyEqrect> sky_call([this](const std::shared_ptr<GxSkyEqrect>& sky) {
        scn->add_skybox(sky);
    });
    GxEndCaller<GxTexture2D> txt_call([sky_call, sky { sky_mgr->create<GxSkyEqrect>(sky_call) }](const std::shared_ptr<GxTexture2D>& t) {
        sky->get_mat_equ()->set_color(t);
    });
    txt_mgr->create_2d_f(reinterpret_cast<const unsigned char* const>(file_content.data()), file_content.size(), txt_call);
}

IblBakerApp::IblBakerApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored end_call([this] {
        uiscn->set_enability(true);
        scn->set_enability(true);
    });

    GxEndCaller<GxUiScene> ui_scn_call([end_call](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxGameScene> scn_call([end_call](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxMesh> msh_call([end_call](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxTextWdg> txw_call([end_call](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxEditWdg> edt_call([end_call](const std::shared_ptr<GxEditWdg>&) {});
    GxEndCaller<GxButton> btn_call([end_call](const std::shared_ptr<GxButton>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, end_call);
    render_engine->set_render_tree(render_tree.get());

    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    auto* const msh_mgr = ast_mgr->get_mesh_manager();

    uiscn = ast_mgr->get_scene_manager()->create<GxUiScene>(ui_scn_call);
    scn = ast_mgr->get_scene_manager()->create<GxGameScene>(scn_call);

    const auto plate_mesh = msh_mgr->create_plate(msh_call);

    auto tmp_txt = mdl_mgr->create<GxTextWdg>(txw_call);
    auto* tmp_tran = tmp_txt->get_transformation();
    tmp_tran->local_scale(0.04f);
    tmp_tran->set_location(GxVec3(0.0f, 0.85f, 0.1f));
    tmp_txt->set_text(L"IBL baker for Gearoenix game engine", end_call);
    uiscn->add_model(tmp_txt);

    tmp_txt = mdl_mgr->create<GxTextWdg>(txw_call);
    tmp_tran = tmp_txt->get_transformation();
    tmp_tran->local_scale(0.03f);
    tmp_tran->set_location(GxVec3(-0.75f, 0.75f, 0.1f));
    tmp_txt->set_text(L"HDR file location:", end_call);
    uiscn->add_model(tmp_txt);

    auto open_button = mdl_mgr->create<GxButton>(btn_call);
    tmp_tran = open_button->get_transformation();
    tmp_tran->local_scale(0.04f);
    tmp_tran->local_x_scale(4.0f);
    tmp_tran->set_location(GxVec3(0.75f, 0.75f, 0.1f));
    open_button->set_text(L"Open File", end_call);
    open_button->set_on_click(std::bind(&IblBakerApp::on_open, this));
    uiscn->add_model(open_button);

    file_location = mdl_mgr->create<GxEditWdg>(edt_call);
    tmp_tran = file_location->get_transformation();
    tmp_tran->local_scale(0.04f);
    tmp_tran->local_x_scale(14.0f);
    tmp_tran->set_location(GxVec3(0.025f, 0.75f, 0.1f));
    file_location->set_hint_text(L"<Fill it with 'file location'>", end_call);
    uiscn->add_model(file_location);
}

IblBakerApp::~IblBakerApp() noexcept
{
    terminate();
}

void IblBakerApp::update() noexcept
{
}

void IblBakerApp::terminate() noexcept
{
}

bool IblBakerApp::on_event(const gearoenix::core::event::Data&) noexcept
{
    return false;
}

GEAROENIX_START(IblBakerApp)
