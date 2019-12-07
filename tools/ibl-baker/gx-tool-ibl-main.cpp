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

//void IblBakerApp::translate_camera(const GxVec3& t)
//{
//    GxVec3 loc = camtrn->get_location() + t;
//    if (loc[0] < -10.0f)
//        loc[0] = -10.0f;
//    else if (loc[0] > 110.0f)
//        loc[0] = 110.0f;
//    if (loc[1] < -10.0f)
//        loc[1] = -10.0f;
//    else if (loc[1] > 60.0f)
//        loc[1] = 60.0f;
//    if (loc[2] < 1.5f)
//        loc[2] = 1.5f;
//    else if (loc[2] > 50.0f)
//        loc[2] = 50.0f;
//    camtrn->set_location(loc);
//}

IblBakerApp::IblBakerApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored endcall([this] {
        //    scn->set_enability(true);
        uiscn->set_enability(true);
        //    auto* const event_engine = system_application->get_event_engine();
        //    event_engine->add_listner(gearoenix::core::event::Id::ButtonMouse, 1.0f, this);
        //    event_engine->add_listner(gearoenix::core::event::Id::ButtonKeyboard, 1.0f, this);
        //    event_engine->add_listner(gearoenix::core::event::Id::MovementMouse, 1.0f, this);
        //    event_engine->add_listner(gearoenix::core::event::Id::ScrollMouse, 1.0f, this);
        //    event_engine->add_listner(gearoenix::core::event::Id::GestureDrag, 1.0f, this);
    });

    //GxEndCaller<GxGameScene> scncall([endcall](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxUiScene> uiscncall([endcall](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxMesh> mshcall([endcall](const std::shared_ptr<GxMesh>&) {});
    //GxEndCaller<GxStaticModel> mdlcall([endcall](const std::shared_ptr<GxStaticModel>&) {});
    //GxEndCaller<GxModal> mdacall([endcall](const std::shared_ptr<GxModal>&) {});
    GxEndCaller<GxTextWdg> txwcall([endcall](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxEditWdg> edtcall([endcall](const std::shared_ptr<GxEditWdg>&) {});
    //GxEndCaller<GxTexture> txtcall([endcall](const std::shared_ptr<GxTexture>&) {});
    GxEndCaller<GxButton> btncall([endcall](const std::shared_ptr<GxButton>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, endcall);
    render_engine->set_render_tree(render_tree.get());

    auto* const astmgr = sys_app->get_asset_manager();
    auto* const mdlmgr = astmgr->get_model_manager();
    auto* const mshmgr = astmgr->get_mesh_manager();

    //scn = astmgr->get_scene_manager()->create<GxGameScene>(scncall);
    uiscn = astmgr->get_scene_manager()->create<GxUiScene>(uiscncall);

    const auto plate_mesh = mshmgr->create_plate(mshcall);

    //const auto& txtmgr = astmgr->get_texture_manager();
    //const std::shared_ptr<GxMesh> msh = mshmgr->create_icosphere(mshcall);
    //const std::shared_ptr<GxMesh> cube = mshmgr->create_cube(mshcall);
    //{
    //    const std::shared_ptr<GxMesh> plate_mesh = mshmgr->create({ GxVertex { GxVec3(-55.0f, -30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(0.0f, 0.0f) },
    //                                                                  GxVertex { GxVec3(55.0f, -30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(110.0f, 0.0f) },
    //                                                                  GxVertex { GxVec3(-55.0f, 30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(0.0f, 60.0f) },
    //                                                                  GxVertex { GxVec3(55.0f, 30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(110.0f, 60.0f) } },
    //        { 0, 1, 2, 1, 3, 2 }, GxAabb3(GxVec3(60.0f), GxVec3(-60.0f)), mshcall);
    //    auto txt = std::dynamic_pointer_cast<GxTexture2D>(txtmgr->get_gx3d(1031, txtcall));
    //    const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
    //    mat->set_roughness_factor(0.5f);
    //    mat->set_metallic_factor(0.8f);
    //    mat->set_color(txt);
    //    const std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
    //    mdl->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, mat));
    //    auto* trans = mdl->get_transformation();
    //    trans->set_location(GxVec3(50.0f, 25.0f, 0.0f));
    //    trans->local_scale(1.0f);
    //    scn->add_model(mdl);
    //}
    //{

    //    const GxReal scale = 0.025f;
    //    const GxReal scalex = lengthx * 0.5f / scale;
    //    const GxReal scaley = (shelf_thickness * 0.5f + 0.5f) / scale;
    //    const GxReal scalez = (shelf_height * 1.5f + marginz * 2.0f) / scale;
    //    GxReal y = margin + shelf_thickness * 0.5f;
    //    for (int ri = 0; ri < rows_count; ++ri, y += shelf_thickness * 2.0f + margin) {
    //        GxReal z = 0.0f;
    //        for (int zi = 0; zi <= shelf_floors_count; ++zi, z += shelf_height + marginz) {
    //            GxReal x = margin + lengthx * 0.5f;
    //            for (int xi = 0; xi < shelves_in_column_count; ++xi, x += margin + lengthx) {
    //                const GxVec3 position(x, y, z);
    //                std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
    //                const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
    //                mat->set_color(0.3f, 0.3f, 0.3f, endcall);
    //                mat->set_roughness_factor(0.2f);
    //                mat->set_metallic_factor(0.9f);
    //                const auto mdlmsh = std::make_shared<GxMdMesh>(cube, mat);
    //                mdl->add_mesh(mdlmsh);
    //                auto* tran = mdl->get_transformation();
    //                tran->set_location(position);
    //                tran->local_scale(scale);
    //                tran->local_x_scale(scalex);
    //                tran->local_y_scale(scaley);
    //                mdl->set_collider(std::make_unique<GxCldSphere>(position, scale));
    //                scn->add_model(mdl);
    //            }
    //        }
    //        GxReal x = margin;
    //        for (int xi = 0; xi < shelves_in_column_count; ++xi) {
    //            auto fx = [&] {
    //                const GxVec3 position(x, y, shelf_height * 1.5f + marginz);
    //                std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
    //                const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
    //                mat->set_color(0.3f, 0.3f, 0.3f, endcall);
    //                mat->set_roughness_factor(0.2f);
    //                mat->set_metallic_factor(0.9f);
    //                const auto mdlmsh = std::make_shared<GxMdMesh>(cube, mat);
    //                mdl->add_mesh(mdlmsh);
    //                auto* tran = mdl->get_transformation();
    //                tran->set_location(position);
    //                tran->local_scale(scale);
    //                tran->local_y_scale(scaley);
    //                tran->local_z_scale(scalez);
    //                mdl->set_collider(std::make_unique<GxCldSphere>(position, scale));
    //                scn->add_model(mdl);
    //            };
    //            fx();
    //            x += lengthx;
    //            fx();
    //            x += margin;
    //        }
    //    }
    //}

    //modal = mdlmgr->create<GxModal>(mdacall);

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

    //modal->add_child(text1);
    //text_location = mdlmgr->create<GxTextWdg>(txwcall);
    //text_location->set_text(L"{ 0.00, 0.00, 0.00 }", endcall);
    //text_location->set_text_color(0.777f, 0.222f, 0.333f, endcall);
    //auto* const text_location_tran = text_location->get_transformation();
    //text_location_tran->local_scale(0.08f);
    //text_location_tran->set_location(GxVec3(0.0f, -0.35f, 0.1f));
    //modal->add_child(text_location);
    //auto text2 = mdlmgr->create<GxTextWdg>(txwcall);
    //text2->set_text(L"& Object Color", endcall);
    //auto* const tx2tran = text2->get_transformation();
    //tx2tran->local_scale(0.1f);
    //tx2tran->set_location(GxVec3(0.0f, -0.0f, 0.1f));
    //modal->add_child(text2);

    auto open_button = mdlmgr->create<GxButton>(btncall);
    tmptran = open_button->get_transformation();
    tmptran->local_scale(0.05f);
    tmptran->local_x_scale(4.0f);
    tmptran->set_location(GxVec3(0.75f, 0.75f, 0.1f));

    tmptxt = mdlmgr->create<GxTextWdg>(txwcall);
    tmptran = tmptxt->get_transformation();
    tmptran->local_scale(0.03f);
    tmptran->set_location(GxVec3(0.75f, 0.75f, 0.2f));
    tmptxt->set_text_color(0.0f, 0.0f, 0.0f, endcall);
    tmptxt->set_text(L"Open File", endcall);
    open_button->add_child(tmptxt);
    //look_at_button->set_on_click([this] {
    //    if (nullptr != selected_item) {
    //        camtrn->look_at(camtrn->get_location(), selected_item.load()->get_transformation()->get_location(), GxVec3(0.0f, 0.0f, 1.0f));
    //        selected_item = nullptr;
    //    }
    //    });

    const std::shared_ptr<GxMaterial> btnmat(new GxMaterial(render_engine, endcall));
    btnmat->set_color(0.9f, 0.075f, 0.05f, endcall);
    open_button->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, btnmat));

    uiscn->add_model(open_button);

    file_location = mdlmgr->create<GxEditWdg>(edtcall);
    tmptran = file_location->get_transformation();
    tmptran->local_scale(0.05f);
    tmptran->local_x_scale(10.75f);
    tmptran->set_location(GxVec3(0.0f, 0.75f, 0.1f));
    file_location->set_hint_text(L"<Fill it with 'file location'>", endcall);
    uiscn->add_model(file_location);

    //modal->add_child(look_at_button);

    //modal->set_enabled(false);
    //modal->get_transformation()->local_scale(0.5f);
    //modal->set_on_close([this]() noexcept {
    //    last_time_item_detail_modal_closed = std::chrono::high_resolution_clock::now();
    //    modal->set_enabled(false);
    //    selected_item = nullptr;
    //    });

    //uiscn->set_layer(scn->get_layer() + 1.0f);
}

IblBakerApp::~IblBakerApp() noexcept
{
    terminate();
}

void IblBakerApp::update() noexcept
{
    //auto forward = camtrn->get_z_axis() * -camera_forward;
    //forward[2] = 0.0f;
    //auto flength = forward.length();
    //if (flength > 0.001f)
    //    forward /= flength;
    //translate_camera((forward + (camtrn->get_x_axis() * camera_sideward)) * (render_engine->get_delta_time() * 3.0f));
}

void IblBakerApp::terminate() noexcept
{
    //cam = nullptr;
    //camtrn = nullptr;
    //modal = nullptr;
    //text_location = nullptr;
    //scn = nullptr;
    //uiscn = nullptr;
    //render_tree = nullptr;
    //gearoenix::core::Application::terminate();
}

bool IblBakerApp::on_event(const gearoenix::core::event::Data& event_data) noexcept
{
    //switch (event_data.source) {
    //case gearoenix::core::event::Id::ButtonMouse: {
    //    const auto& d = std::get<gearoenix::core::event::button::MouseData>(event_data.data);
    //    if (d.key == gearoenix::core::event::button::MouseKeyId::Left) {
    //        if (d.action == gearoenix::core::event::button::MouseActionId::Click) {
    //            const auto ray = cam->create_ray3(d.position[0], d.position[1]);
    //            auto hit = scn->hit(ray, std::numeric_limits<gearoenix::core::Real>::max());
    //            if (hit.has_value() && std::chrono::duration<GxReal>(std::chrono::high_resolution_clock::now() - last_time_item_detail_modal_closed).count() > 0.5f) {
    //                auto* cld = hit.value().second;
    //                auto* mdl = cld->get_parent();
    //                selected_item.store(reinterpret_cast<GxStaticModel*>(mdl));
    //                auto& mdll = mdl->get_transformation()->get_location();
    //                auto color = *(mdl->get_meshes().begin()->second->get_material()->get_color_value());
    //                std::wstringstream tl;
    //                tl << "{ x: " << mdll[0] << ", y: " << mdll[1] << ", z: " << mdll[2] << " }";
    //                const GxEndCallerIgnored call([] {});
    //                text_location->set_text_color(color[0], color[1], color[2], call);
    //                text_location->set_text(tl.str(), call);
    //                modal->set_enabled(true);
    //            }
    //        }
    //    }
    //    break;
    //}
    //case gearoenix::core::event::Id::MovementMouse: {
    //    break;
    //}
    //case gearoenix::core::event::Id::ButtonKeyboard: {
    //    const auto& d = std::get<gearoenix::core::event::button::KeyboardData>(event_data.data);
    //    switch (d.key) {
    //    case gearoenix::core::event::button::KeyboardKeyId::Up:
    //        if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
    //            camera_forward = 1.0f;
    //        else
    //            camera_forward = 0.0f;
    //        break;
    //    case gearoenix::core::event::button::KeyboardKeyId::Down:
    //        if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
    //            camera_forward = -1.0f;
    //        else
    //            camera_forward = 0.0f;
    //        break;
    //    case gearoenix::core::event::button::KeyboardKeyId::Right:
    //        if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
    //            camera_sideward = 1.0f;
    //        else
    //            camera_sideward = 0.0f;
    //        break;
    //    case gearoenix::core::event::button::KeyboardKeyId::Left:
    //        if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
    //            camera_sideward = -1.0f;
    //        else
    //            camera_sideward = 0.0f;
    //        break;
    //    default:
    //        break;
    //    }
    //    break;
    //}
    //case gearoenix::core::event::Id::ScrollMouse: {
    //    if (selected_item == nullptr) {
    //        translate_camera(camtrn->get_z_axis() * -std::get<gearoenix::core::event::button::MouseScroll>(event_data.data).direction[1]);
    //    }
    //    break;
    //}
    //case gearoenix::core::event::Id::GestureDrag: {
    //    if (selected_item == nullptr) {
    //        const auto& d = std::get<gearoenix::core::event::gesture::Drag>(event_data.data);
    //        const auto& v = d.delta_previous_position;
    //        camtrn->local_x_rotate(v[1]);
    //        camtrn->global_rotate(v[0], GxVec3(0.0f, 0.0f, 1.0f));
    //    }
    //    break;
    //}
    //default:
    //    break;
    //}
    return false;
}

GEAROENIX_START(IblBakerApp)
