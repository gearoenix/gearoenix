#include "exm-003-ui-widgets.hpp"
#include <gearoenix/core/asset/cr-asset-manager.hpp>
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
using GxVec3 = gearoenix::math::Vec3;

Example003UiWidgetsApp::Example003UiWidgetsApp(gearoenix::system::Application* const sys_app) noexcept
    : gearoenix::core::Application::Application(sys_app)
{
    const GxEndCallerIgnored endcall([this] {
        uiscn->set_enability(true);
    });

    GxEndCaller<GxUiScene> uiscncall([endcall](const std::shared_ptr<GxUiScene>&) {});
    GxEndCaller<GxTextWdg> txwcall([endcall](const std::shared_ptr<GxTextWdg>&) {});
    GxEndCaller<GxEditWdg> edtcall([endcall](const std::shared_ptr<GxEditWdg>&) {});
    GxEndCaller<GxButton> btncall([endcall](const std::shared_ptr<GxButton>&) {});

    render_tree = std::make_unique<GxGrPbr>(render_engine, endcall);
    render_engine->set_render_tree(render_tree.get());

    auto* const astmgr = sys_app->get_asset_manager();
    auto* const mdlmgr = astmgr->get_model_manager();
    auto* const scnmgr = astmgr->get_scene_manager();

    uiscn = scnmgr->create<GxUiScene>(uiscncall);

    auto text = mdlmgr->create<GxTextWdg>(txwcall);
    auto* tmptran = text->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->set_location(GxVec3(0.0f, 0.85f, 0.0f));
    text->set_text(L"I'm a one line Text widget. I easily grow horizontally and automatically.", endcall);
    uiscn->add_model(text);

    auto button = mdlmgr->create<GxButton>(btncall);
    tmptran = button->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->set_location(GxVec3(0.0f, 0.75f, 0.0f));
    button->set_text(L"I'm a Button widget. I can grow too.", endcall);
    uiscn->add_model(button);

    auto edit = mdlmgr->create<GxEditWdg>(edtcall);
    tmptran = edit->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->local_x_scale(34.0f);
    tmptran->set_location(GxVec3(0.0f, 0.65f, 0.0f));
    edit->set_hint_text(L"I'm a one line Edit text widget. I don't grow, there is a reason behind it, don't you know it.", endcall);
    uiscn->add_model(edit);

    auto edit2 = mdlmgr->create<GxEditWdg>(edtcall);
    tmptran = edit2->get_transformation();
    tmptran->local_scale(0.04f);
    tmptran->local_x_scale(20.0f);
    tmptran->set_location(GxVec3(0.0f, 0.55f, 0.0f));
    edit2->set_hint_text(L"I'm another Edit text widget with smaller limit.", endcall);
    uiscn->add_model(edit2);
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
    uiscn = nullptr;
    render_tree = nullptr;
    gearoenix::core::Application::terminate();
}

bool Example003UiWidgetsApp::on_event(const gearoenix::core::event::Data&) noexcept
{
    return false;
}

GEAROENIX_START(Example003UiWidgetsApp)
