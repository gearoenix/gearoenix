#include "exm-003-net-hello-world.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/event/cr-ev-engine.hpp>
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
#include <gearoenix/render/widget/rnd-wdg-modal.hpp>
#include <gearoenix/render/widget/rnd-wdg-text.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <sstream>

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
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;
using GxTexture = gearoenix::render::texture::Texture;
using GxTexture2D = gearoenix::render::texture::Texture2D;
using GxCldSphere = gearoenix::physics::collider::Sphere;
using GxVertex = gearoenix::math::BasicVertex;

void GameApp::translate_camera(const GxVec3& t)
{
    GxVec3 loc = camtrn->get_location() + t;
    if (loc[0] < -10.0f)
        loc[0] = -10.0f;
    else if (loc[0] > 110.0f)
        loc[0] = 110.0f;
    if (loc[1] < -10.0f)
        loc[1] = -10.0f;
    else if (loc[1] > 60.0f)
        loc[1] = 60.0f;
    if (loc[2] < 1.5f)
        loc[2] = 1.5f;
    else if (loc[2] > 50.0f)
        loc[2] = 50.0f;
    camtrn->set_location(loc);
}

GameApp::GameApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    std::random_device rand_dev;
    std::default_random_engine rand_eng(rand_dev());
    std::uniform_real_distribution<GxReal> rand_gen1(0.1f, 0.2f);
    std::uniform_real_distribution<GxReal> rand_gen2(0.001f, 0.999f);
    std::uniform_int_distribution<int> rand_gen3(10, 15);

    constexpr GxReal margin = 3.0f;
    constexpr GxReal marginz = 0.2f;
    constexpr GxReal lengthx = 30.0f;
    constexpr int shelf_floors_count = 3;
    constexpr GxReal shelf_height = 1.5f;
    constexpr GxReal shelf_thickness = 1.0f;
    constexpr int rows_count = 10;
    constexpr int shelves_in_row_count = 2;
    constexpr int shelves_in_column_count = 3;
    constexpr int shelves_count = shelves_in_row_count * shelves_in_column_count * shelf_floors_count * rows_count;

    ShelfInfo shelves_info[shelves_count];

    GxReal y = margin;
    for (int rowi = 0, i = 0; rowi < rows_count; ++rowi, y += margin) {
        for (int shelfi = 0; shelfi < shelves_in_row_count; ++shelfi, y += shelf_thickness) {
            GxReal x = margin;
            for (int ci = 0; ci < shelves_in_column_count; ++ci, x += margin) {
                const GxReal endx = x + lengthx;
                GxReal z = marginz;
                for (int zi = 0; zi < shelf_floors_count; ++i, ++zi, z += marginz) {
                    const GxReal endz = z + shelf_height;
                    auto& shelf_info = shelves_info[i];
                    shelf_info.y = y;
                    shelf_info.rand_genx = std::uniform_real_distribution<GxReal>(x, endx);
                    shelf_info.rand_genz = std::uniform_real_distribution<GxReal>(z, endz);
                    z = endz;
                }
                x = endx;
            }
        }
    }

    const GxEndCallerIgnored endcall([this] {
        scn->set_enability(true);
        uiscn->set_enability(true);
        auto* const event_engine = system_application->get_event_engine();
        event_engine->add_listner(gearoenix::core::event::Id::ButtonMouse, -1.0f, this);
        event_engine->add_listner(gearoenix::core::event::Id::ButtonKeyboard, 1.0f, this);
        event_engine->add_listner(gearoenix::core::event::Id::MovementMouse, 1.0f, this);
        event_engine->add_listner(gearoenix::core::event::Id::ScrollMouse, 1.0f, this);
        event_engine->add_listner(gearoenix::core::event::Id::GestureDrag, 1.0f, this);
    });

    GxEndCaller<GxGameScene> scncall([endcall](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxUiScene> uiscncall([endcall](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxMesh> mshcall([endcall](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxStaticModel> mdlcall([endcall](const std::shared_ptr<GxStaticModel>&) {});
    GxEndCaller<GxModal> mdacall([endcall](const std::shared_ptr<GxModal>&) {});
    GxEndCaller<GxTextWdg> txwcall([endcall](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxTexture> txtcall([endcall](const std::shared_ptr<GxTexture>&) {});
    GxEndCaller<GxButton> btncall([endcall](const std::shared_ptr<GxButton>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, endcall);
    render_engine->set_render_tree(render_tree.get());
    gearoenix::core::asset::Manager* const astmgr = sys_app->get_asset_manager();
    scn = astmgr->get_scene_manager()->create<GxGameScene>(scncall);
    uiscn = astmgr->get_scene_manager()->create<GxUiScene>(uiscncall);

    cam = astmgr->get_camera_manager()->create<GxPersCam>();
    camtrn = std::static_pointer_cast<GxCamTran>(cam->get_transformation());
    camtrn->look_at(GxVec3(2.0f, 25.0f, 1.5f), GxVec3(50.0f, 25.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f));
    cam->set_far(200.0f);
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

    const auto& mshmgr = astmgr->get_mesh_manager();
    const auto& txtmgr = astmgr->get_texture_manager();
    const std::shared_ptr<GxMesh> msh = mshmgr->create_icosphere(mshcall);
    const std::shared_ptr<GxMesh> cube = mshmgr->create_cube(mshcall);
    const std::shared_ptr<GxMdManager>& mdlmgr = astmgr->get_model_manager();
    {
        const std::shared_ptr<GxMesh> plate_mesh = mshmgr->create({ GxVertex { GxVec3(-55.0f, -30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(0.0f, 0.0f) },
                                                                      GxVertex { GxVec3(55.0f, -30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(110.0f, 0.0f) },
                                                                      GxVertex { GxVec3(-55.0f, 30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(0.0f, 60.0f) },
                                                                      GxVertex { GxVec3(55.0f, 30.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f), GxVec4(1.0f, 0.0f, 0.0f, 1.0f), GxVec2(110.0f, 60.0f) } },
            { 0, 1, 2,
                1, 3, 2 },
            60.0f, mshcall);
        auto txt = std::dynamic_pointer_cast<GxTexture2D>(txtmgr->get_gx3d(1031, txtcall));
        const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
        mat->set_roughness_factor(0.5f);
        mat->set_metallic_factor(0.8f);
        mat->set_color(txt);
        const std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
        mdl->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, mat));
        auto* trans = mdl->get_transformation();
        trans->set_location(GxVec3(50.0f, 25.0f, 0.0f));
        trans->local_scale(1.0f);
        scn->add_model(mdl);
    }
    {
        const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
        mat->set_color(1.0f, 0.0f, 0.0, endcall);
        const std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
        mdl->add_mesh(std::make_shared<GxMdMesh>(cube, mat));
        auto* tran = mdl->get_transformation();
        const GxVec3 position(50.0f, 25.0f, 20.0f);
        const auto scale = 5.0f;
        tran->set_location(position);
        tran->local_scale(scale);
        mdl->set_collider(std::make_unique<GxCldSphere>(position, scale));
        scn->add_model(mdl);
    }
    for (auto& s : shelves_info) {
        const int items_count = rand_gen3(rand_eng);
        for (int i = 0; i < items_count; ++i) {
            const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
            mat->set_roughness_factor(rand_gen2(rand_eng));
            mat->set_metallic_factor(rand_gen2(rand_eng));
            mat->set_color(rand_gen2(rand_eng), rand_gen2(rand_eng), rand_gen2(rand_eng), endcall);
            const std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
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
    auto text1 = mdlmgr->create<GxTextWdg>(txwcall);
    text1->set_text(L"Object Location", endcall);
    auto* const tx1tran = text1->get_transformation();
    tx1tran->local_scale(0.1f);
    tx1tran->set_location(GxVec3(0.0f, 0.25f, 0.1f));
    modal->add_child(text1);
    text_location = mdlmgr->create<GxTextWdg>(txwcall);
    text_location->set_text(L"{ 0.00, 0.00, 0.00 }", endcall);
    text_location->set_text_color(0.777f, 0.222f, 0.333f, endcall);
    auto* const text_location_tran = text_location->get_transformation();
    text_location_tran->local_scale(0.08f);
    text_location_tran->set_location(GxVec3(0.0f, -0.35f, 0.1f));
    modal->add_child(text_location);
    auto text2 = mdlmgr->create<GxTextWdg>(txwcall);
    text2->set_text(L"& Object Color", endcall);
    auto* const tx2tran = text2->get_transformation();
    tx2tran->local_scale(0.1f);
    tx2tran->set_location(GxVec3(0.0f, -0.0f, 0.1f));
    modal->add_child(text2);

    look_at_button = mdlmgr->create<GxButton>(btncall);
    auto* const look_at_button_tran = look_at_button->get_transformation();
    look_at_button_tran->local_scale(0.1f);
    look_at_button_tran->local_x_scale(4.0f);
    look_at_button_tran->set_location(GxVec3(-0.5f, -0.8f, 0.1f));

    auto look_at_text = mdlmgr->create<GxTextWdg>(txwcall);
    look_at_text->set_text(L"Look At Object", endcall);
    auto* const look_at_text_tran = look_at_text->get_transformation();
    look_at_text_tran->local_scale(0.06f);
    look_at_text_tran->set_location(GxVec3(-0.5f, -0.815f, 0.2f));
    look_at_button->add_child(look_at_text);

    const auto plate_mesh = mshmgr->create_plate(mshcall);
    const std::shared_ptr<GxMaterial> btnmat(new GxMaterial(render_engine, endcall));
    btnmat->set_color(0.9f, 0.075f, 0.05f, endcall);
    look_at_button->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, btnmat));

    modal->add_child(look_at_button);

    modal->set_enability(gearoenix::core::State::Unset);
    modal->get_transformation()->local_scale(0.5f);
    modal->set_on_close([this]() noexcept {
        showing_object_details = false;
        modal->set_enability(gearoenix::core::State::Unset);
    });

    uiscn->add_model(modal);
    uiscn->set_layer(scn->get_layer() + 1.0f);
}

GameApp::~GameApp() noexcept
{
    terminate();
}

void GameApp::update() noexcept
{
    auto forward = camtrn->get_z_axis() * -camera_forward;
    forward[2] = 0.0f;
    auto flength = forward.length();
    if (flength > 0.001f)
        forward /= flength;
    translate_camera((forward + (camtrn->get_x_axis() * camera_sideward)) * (render_engine->get_delta_time() * 3.0f));
}

void GameApp::terminate() noexcept
{
    cam = nullptr;
    camtrn = nullptr;
    modal = nullptr;
    text_location = nullptr;
    scn = nullptr;
    uiscn = nullptr;
    render_tree = nullptr;
    gearoenix::core::Application::terminate();
}

bool GameApp::on_event(const gearoenix::core::event::Data& event_data) noexcept
{
    switch (event_data.source) {
    case gearoenix::core::event::Id::ButtonMouse: {
        const auto& d = std::get<gearoenix::core::event::button::MouseData>(event_data.data);
        if (d.key == gearoenix::core::event::button::MouseKeyId::Left) {
            if (d.action == gearoenix::core::event::button::MouseActionId::Click) {
                const auto ray = cam->create_ray3(d.position[0], d.position[1]);
                auto hit = scn->hit(ray, std::numeric_limits<gearoenix::core::Real>::max());
                if (hit.has_value() && !showing_object_details.exchange(true)) {
                    auto* cld = hit.value().second;
                    auto* mdl = cld->get_parent();
                    auto& mdll = mdl->get_occlusion_sphere().get_center();
                    auto color = *(mdl->get_meshes().begin()->second->get_material()->get_color());
                    std::wstringstream tl;
                    tl << "{ x: " << mdll[0] << ", y: " << mdll[1] << ", z: " << mdll[2] << " }";
                    const GxEndCallerIgnored call([] {});
                    text_location->set_text_color(color[0], color[1], color[2], call);
                    text_location->set_text(tl.str(), call);
                    modal->set_enability(gearoenix::core::State::Set);
                }
            }
        }
        break;
    }
    case gearoenix::core::event::Id::MovementMouse: {
        break;
    }
    case gearoenix::core::event::Id::ButtonKeyboard: {
        const auto& d = std::get<gearoenix::core::event::button::KeyboardData>(event_data.data);
        switch (d.key) {
        case gearoenix::core::event::button::KeyboardKeyId::Up:
            if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
                camera_forward = 1.0f;
            else
                camera_forward = 0.0f;
            break;
        case gearoenix::core::event::button::KeyboardKeyId::Down:
            if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
                camera_forward = -1.0f;
            else
                camera_forward = 0.0f;
            break;
        case gearoenix::core::event::button::KeyboardKeyId::Right:
            if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
                camera_sideward = 1.0f;
            else
                camera_sideward = 0.0f;
            break;
        case gearoenix::core::event::button::KeyboardKeyId::Left:
            if (d.action == gearoenix::core::event::button::KeyboardActionId::Press)
                camera_sideward = -1.0f;
            else
                camera_sideward = 0.0f;
            break;
        default:
            break;
        }
        break;
    }
    case gearoenix::core::event::Id::ScrollMouse: {
        if (!showing_object_details)
            translate_camera(camtrn->get_z_axis() * -std::get<gearoenix::core::event::button::MouseScroll>(event_data.data).direction[1]);
        break;
    }
    case gearoenix::core::event::Id::GestureDrag: {
        if (!showing_object_details) {
            const auto& d = std::get<gearoenix::core::event::gesture::Drag>(event_data.data);
            const auto& v = d.delta_previous_position;
            camtrn->local_x_rotate(v[1]);
            camtrn->global_rotate(v[0], GxVec3(0.0f, 0.0f, 1.0f));
            break;
        }
    }
    default:
        break;
    }
    return false;
}

GEAROENIX_START(GameApp)
