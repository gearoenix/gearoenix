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
#include <gearoenix/render/mesh/rnd-msh-manager.hpp>
#include <gearoenix/render/mesh/rnd-msh-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/model/rnd-mdl-mesh.hpp>
#include <gearoenix/render/model/rnd-mdl-static.hpp>
#include <gearoenix/render/scene/rnd-scn-game.hpp>
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
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

IblBakerApp::IblBakerApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored endcall([this] {
        uiscn->set_enability(true);
    });

    GxEndCaller<GxUiScene> uiscncall([endcall](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxMesh> mshcall([endcall](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxTextWdg> txwcall([endcall](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxEditWdg> edtcall([endcall](const std::shared_ptr<GxEditWdg>&) {});
    GxEndCaller<GxButton> btncall([endcall](const std::shared_ptr<GxButton>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, endcall);
    render_engine->set_render_tree(render_tree.get());

    auto* const astmgr = sys_app->get_asset_manager();
    auto* const mdlmgr = astmgr->get_model_manager();
    auto* const mshmgr = astmgr->get_mesh_manager();

    uiscn = astmgr->get_scene_manager()->create<GxUiScene>(uiscncall);

    const auto plate_mesh = mshmgr->create_plate(mshcall);

    auto tmptxt = mdlmgr->create<GxTextWdg>(txwcall);
    auto* tmptran = tmptxt->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->set_location(GxVec3(0.0f, 0.85f, 0.1f));
    tmptxt->set_text(L"IBL baker for Gearoenix game engine", endcall);
    uiscn->add_model(tmptxt);

    tmptxt = mdlmgr->create<GxTextWdg>(txwcall);
    tmptran = tmptxt->get_transformation();
    tmptran->local_scale(0.03f);
    tmptran->set_location(GxVec3(-0.75f, 0.75f, 0.1f));
    tmptxt->set_text(L"HDR file location:", endcall);
    uiscn->add_model(tmptxt);

    auto open_button = mdlmgr->create<GxButton>(btncall);
    tmptran = open_button->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->local_x_scale(4.0f);
    tmptran->set_location(GxVec3(0.75f, 0.75f, 0.1f));
    open_button->set_text(L"Open File", endcall);
    uiscn->add_model(open_button);

    file_location = mdlmgr->create<GxEditWdg>(edtcall);
    tmptran = file_location->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->local_x_scale(14.0f);
    tmptran->set_location(GxVec3(0.025f, 0.75f, 0.1f));
    file_location->set_hint_text(L"<Fill it with 'file location'>", endcall);
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
