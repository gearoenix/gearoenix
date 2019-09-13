#include "exm-003-net-hello-world.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/core/event/cr-ev-engine.hpp>
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
#include <gearoenix/render/widget/rnd-wdg-modal.hpp>
#include <gearoenix/render/widget/rnd-wdg-text.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <random>
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
using GxVec3 = gearoenix::math::Vec3;
using GxReal = gearoenix::core::Real;
using GxDirLight = gearoenix::render::light::Directional;
using GxLtManager = gearoenix::render::light::Manager;
using GxPersCam = gearoenix::render::camera::Perspective;
using GxCldSphere = gearoenix::physics::collider::Sphere;

struct ShelfInfo {
    std::uniform_real_distribution<GxReal> rand_genx;
    std::uniform_real_distribution<GxReal> rand_genz;
    GxReal y = 0.0f;
};

GameApp::GameApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    std::random_device rand_dev;
    std::default_random_engine rand_eng(rand_dev());
    std::uniform_real_distribution<GxReal> rand_gen1(0.2f, 0.4f);
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
        system_application->get_event_engine()->add_listner(gearoenix::core::event::Id::ButtonMouse, 1.0f, this);
    });
    GxEndCaller<GxGameScene> scncall([endcall](const std::shared_ptr<GxGameScene>&) {});
    GxEndCaller<GxUiScene> uiscncall([endcall](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxMesh> mshcall([endcall](const std::shared_ptr<GxMesh>&) {});
    GxEndCaller<GxStaticModel> mdlcall([endcall](const std::shared_ptr<GxStaticModel>&) {});
    GxEndCaller<GxModal> mdacall([endcall](const std::shared_ptr<GxModal>&) {});
    GxEndCaller<GxTextWdg> txwcall([endcall](const std::shared_ptr<GxTextWdg>&) {});
    render_tree = std::make_unique<GxGrPbr>(render_engine, endcall);
    render_engine->set_render_tree(render_tree.get());
    gearoenix::core::asset::Manager* const astmgr = sys_app->get_asset_manager();
    scn = astmgr->get_scene_manager()->create<GxGameScene>(scncall);
    uiscn = astmgr->get_scene_manager()->create<GxUiScene>(uiscncall);

    cam = astmgr->get_camera_manager()->create<GxPersCam>();
    camtrn = std::static_pointer_cast<GxCamTran>(cam->get_transformation());
    camtrn->look_at(GxVec3(-10.0f, -10.0f, 40.0f), GxVec3(50.0f, 25.0f, 0.0f), GxVec3(0.0f, 0.0f, 1.0f));
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

    const std::shared_ptr<GxMesh> msh = astmgr->get_mesh_manager()->create_icosphere(mshcall);
    const std::shared_ptr<GxMesh> plate_mesh = astmgr->get_mesh_manager()->create_plate(mshcall);
    const std::shared_ptr<GxMdManager>& mdlmgr = astmgr->get_model_manager();
    {
        const std::shared_ptr<GxMaterial> mat(new GxMaterial(render_engine, endcall));
        mat->set_roughness_factor(0.5f);
        mat->set_metallic_factor(0.8f);
        mat->set_color(0.0f, 0.999f, 0.0f, endcall);
        const std::shared_ptr<GxStaticModel> mdl = mdlmgr->create<GxStaticModel>(mdlcall);
        mdl->add_mesh(std::make_shared<GxMdMesh>(plate_mesh, mat));
        auto* trans = mdl->get_transformation();
        trans->set_location(GxVec3(0.0f, 0.0f, -5.0f));
        trans->local_scale(14.0f);
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
    text1->set_text(L"Object Location");
    auto* const tx1tran = text1->get_transformation();
    tx1tran->local_scale(0.1f);
    tx1tran->set_location(GxVec3(0.0f, 0.25f, 0.1f));
    modal->add_child(text1);
    text_location = mdlmgr->create<GxTextWdg>(txwcall);
    text_location->set_text(L"{ 0.00, 0.00, 0.00 }");
    text_location->set_text_color(0.777f, 0.222f, 0.333f);
    auto* const text_location_tran = text_location->get_transformation();
    text_location_tran->local_scale(0.1f);
    text_location_tran->set_location(GxVec3(0.0f, -0.35f, 0.1f));
    modal->add_child(text_location);
    auto text2 = mdlmgr->create<GxTextWdg>(txwcall);
    text2->set_text(L"& Object Color");
    auto* const tx2tran = text2->get_transformation();
    tx2tran->local_scale(0.1f);
    tx2tran->set_location(GxVec3(0.0f, -0.0f, 0.1f));
    modal->add_child(text2);
    modal->set_enability(gearoenix::core::State::Unset);
    modal->get_transformation()->local_scale(0.5f);
    uiscn->add_model(modal);
    uiscn->set_layer(scn->get_layer() + 1.0f);
}

void GameApp::update() noexcept
{
    camtrn->global_rotate(render_engine->get_delta_time() * 0.06f, GxVec3(0.0f, 0.0f, 1.0f), GxVec3(50.0f, 25.0f, 0.0f));
}

void GameApp::terminate() noexcept
{
    gearoenix::core::Application::terminate();
    scn = nullptr;
    uiscn = nullptr;
    cam = nullptr;
    camtrn = nullptr;
    modal = nullptr;
    text_location = nullptr;
    render_tree = nullptr;
}

bool GameApp::on_event(const gearoenix::core::event::Data& event_data) noexcept
{
    switch (event_data.source) {
    case gearoenix::core::event::Id::ButtonMouse: {
        const auto d = std::get<gearoenix::core::event::button::Data>(event_data.data);
        if (d.key == gearoenix::core::event::button::KeyId::Left && d.action == gearoenix::core::event::button::ActionId::Press) {
            const auto ray = cam->create_ray3(d.x, d.y);
            auto hit = scn->hit(ray, std::numeric_limits<gearoenix::core::Real>::max());
            if (hit.has_value()) {
                modal->set_enability(gearoenix::core::State::Unset);
                auto* cld = hit.value().second;
                auto* mdl = cld->get_parent();
                auto& mdll = mdl->get_occlusion_sphere().get_center();
                auto color = *(mdl->get_meshes().begin()->second->get_material()->get_color());
                std::wstringstream tl;
                tl << "{ " << mdll[0] << ", " << mdll[1] << ", " << mdll[2] << " }";
                GxEndCallerIgnored call([this] {
                    modal->set_enability(gearoenix::core::State::Set);
                });
                text_location->set_text_color(color[0], color[1], color[2], call);
                text_location->set_text(tl.str(), call);
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
