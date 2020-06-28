#include "exm-003-ui-widgets.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/physics/phs-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/render/graph/tree/rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/rnd-scn-manager.hpp>
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
#include <gearoenix/render/widget/rnd-wdg-button.hpp>
#include <gearoenix/render/widget/rnd-wdg-edit.hpp>
#include <gearoenix/render/widget/rnd-wdg-text.hpp>
#include <gearoenix/system/sys-app.hpp>

template <class T>
using GxEndCaller = gearoenix::core::sync::EndCaller<T>;
using GxEndCallerIgnore = gearoenix::core::sync::EndCallerIgnore;
using GxEndCallerIgnored = GxEndCaller<GxEndCallerIgnore>;
using GxMdManager = gearoenix::render::model::Manager;
using GxButton = gearoenix::render::widget::Button;
using GxEditWdg = gearoenix::render::widget::Edit;
using GxTextWdg = gearoenix::render::widget::Text;
using GxVec3 = gearoenix::math::Vec3<double>;

Example003UiWidgetsApp::Example003UiWidgetsApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored end_call([this] {
        ui_scene->set_enability(true);
    });

    GxEndCaller<GxUiScene> ui_scn_call([end_call](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxTextWdg> txw_call([end_call](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxEditWdg> edt_call([end_call](const std::shared_ptr<GxEditWdg>&) {});
    GxEndCaller<GxButton> btn_call([end_call](const std::shared_ptr<GxButton>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, end_call);
    render_engine->set_render_tree(render_tree.get());

    auto* const ast_mgr = sys_app->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    auto* const scn_mgr = ast_mgr->get_scene_manager();

    ui_scene = scn_mgr->create<GxUiScene>("ui", ui_scn_call);

    auto text = mdl_mgr->create<GxTextWdg>("txt-1", txw_call);
    auto* tmp_tran = text->get_transformation();
    tmp_tran->local_scale(0.04f);
    tmp_tran->set_location(GxVec3(0.0, 0.85, 0.0));
    text->set_text(L"I'm an one line Text widget. I easily grow horizontally and automatically.", end_call);
    ui_scene->add_model(text);

    auto button = mdl_mgr->create<GxButton>("btn-1", btn_call);
    tmp_tran = button->get_transformation();
    tmp_tran->local_scale(0.04);
    tmp_tran->set_location(GxVec3(0.0, 0.75, 0.0));
    button->set_text(L"I'm a Button widget. I can grow too.", end_call);
    ui_scene->add_model(button);

    auto edit = mdl_mgr->create<GxEditWdg>("edt-1", edt_call);
    tmp_tran = edit->get_transformation();
    tmp_tran->local_scale(0.04);
    tmp_tran->local_x_scale(34.0);
    tmp_tran->set_location(GxVec3(0.0, 0.65, 0.0));
    edit->set_hint_text(L"I'm an one line Edit text widget. I don't grow, there is a reason behind it, don't you know it.", end_call);
    ui_scene->add_model(edit);

    auto edit2 = mdl_mgr->create<GxEditWdg>("edt-2", edt_call);
    tmp_tran = edit2->get_transformation();
    tmp_tran->local_scale(0.04);
    tmp_tran->local_x_scale(20.0);
    tmp_tran->set_location(GxVec3(0.0, 0.55, 0.0));
    edit2->set_hint_text(L"I'm another Edit text widget with smaller limit.", end_call);
    ui_scene->add_model(edit2);
}

Example003UiWidgetsApp::~Example003UiWidgetsApp() noexcept
{
    terminate();
}

void Example003UiWidgetsApp::update() noexcept
{
}

void Example003UiWidgetsApp::terminate() noexcept
{
    ui_scene = nullptr;
    render_tree = nullptr;
    gearoenix::core::Application::terminate();
}

bool Example003UiWidgetsApp::on_event(const gearoenix::core::event::Data&) noexcept
{
    return false;
}

GEAROENIX_START(Example003UiWidgetsApp)
