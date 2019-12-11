#include "rnd-wdg-edit.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-string.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/animation/phs-anm-manager.hpp"
#include "../../physics/collider/phs-cld-aabb.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-dynamic.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include <cmath>
#include <string>

#define GX_EDIT_INIT Widget(my_id, Type::Edit, e, c), text_material(new material::Material(e, c)), hint_text_material(new material::Material(e, c)), background_material(new material::Material(e, c)), cursor_material(new material::Material(e, c))

void gearoenix::render::widget::Edit::init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& endcall) noexcept
{
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c([endcall, this] {
        auto* const ee = e->get_system_application()->get_event_engine();
        ee->add_listner(core::event::Id::ButtonKeyboard, 0.0f, this);
    });

    set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0f, 1.0f, 0.001f), math::Vec3(-1.0f, -1.0f, -0.001f)));

    auto* const astmgr = e->get_system_application()->get_asset_manager();
    auto* const mshmgr = astmgr->get_mesh_manager();
    auto* const mdlmgr = astmgr->get_model_manager();
    auto* const fntmgr = astmgr->get_font_manager();

    core::sync::EndCaller<mesh::Mesh> mshend([c](const std::shared_ptr<mesh::Mesh>&) {});
    const auto plate_mesh = mshmgr->create_plate(mshend);

    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = fntmgr->get_default_2d(fend);

    text_material->set_translucency(material::TranslucencyMode::Transparent);
    hint_text_material->set_translucency(material::TranslucencyMode::Transparent);

    core::sync::EndCaller<model::Dynamic> mdlend([c](const std::shared_ptr<model::Dynamic>&) {});

    text_model = mdlmgr->create<model::Dynamic>(mdlend);
    text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, text_material));
    text_model->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    text_model->set_enabled(false);
    add_child(text_model);

    hint_text_model = mdlmgr->create<model::Dynamic>(mdlend);
    hint_text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, hint_text_material));
    hint_text_model->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    hint_text_model->set_enabled(false);
    add_child(hint_text_model);

    background_material->set_color(0.02f, 0.02f, 0.02f, c);
    background_model = mdlmgr->create<model::Dynamic>(mdlend);
    background_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, background_material));
    add_child(background_model);

    cursor_material->set_color(0.0f, 0.0f, 0.0f, c);
    cursor_material->set_translucency(material::TranslucencyMode::Transparent);
    cursor_model = mdlmgr->create<model::Dynamic>(mdlend);
    cursor_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, cursor_material));
    auto* const cursor_tran = cursor_model->get_transformation();
    cursor_tran->set_location(math::Vec3(0.0f, 0.0f, 0.02f));
    cursor_tran->local_scale(theme.cursor_size);
    add_child(cursor_model);

    const auto t1 = theme.cursor_state_time;
    const auto t2 = theme.cursor_blink_time;
    const auto t3 = theme.cursor_state_time + theme.cursor_blink_time;
    const auto t4 = t3 + theme.cursor_state_time;
    const auto t5 = t4 + theme.cursor_blink_time;

    cursor_animation = std::make_shared<physics::animation::Animation>(
        [cursor_material { cursor_material }, t1, t2, t3, t4, t5](
            const core::Real from_start, const core::Real) noexcept {
            const auto s = from_start - std::floor(from_start / t5) * t5;
            if (t1 > s) {
                cursor_material->set_alpha(1.0f);
            } else if (t3 > s) {
                cursor_material->set_alpha(1.0f - (s - t1) / t2);
            } else if (t4 > s) {
                cursor_material->set_alpha(0.0f);
            } else {
                cursor_material->set_alpha((s - t4) / t2);
            }
        },
        std::numeric_limits<core::Real>::max());
    e->get_physics_engine()->get_animation_manager()->add(cursor_animation);

    collider->set_on_scale(std::bind(&Edit::on_scale, this));
}

void gearoenix::render::widget::Edit::on_scale() noexcept
{
    if (text.empty()) {
        if (!hint_text.empty()) {
            set_hint_text(text);
        }
    } else {
        set_text(text);
    }
    auto cursor_scale = static_cast<core::Real>(theme.cursor_width) * 0.5f;
    cursor_scale /= static_cast<core::Real>(e->get_system_application()->get_window_width());
    cursor_scale /= cursor_model->get_collider()->get_current_local_scale()[0];
    cursor_model->get_transformation()->local_x_scale(cursor_scale);
}

gearoenix::render::widget::Edit::Edit(
    const core::Id my_id,
    system::stream::Stream* const,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : GX_EDIT_INIT {
        GXUNIMPLEMENTED
    }

    gearoenix::render::widget::Edit::Edit(const core::Id my_id, engine::Engine* const e, const Theme& theme, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : GX_EDIT_INIT
    , theme(theme)
{
    init(c);
}

gearoenix::render::widget::Edit::Edit(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : GX_EDIT_INIT
{
    init(c);
}

gearoenix::render::widget::Edit::~Edit() noexcept
{
    cursor_animation->set_activity(false);
}

void gearoenix::render::widget::Edit::set_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    if (t.empty())
        return;
    text = t;
    const auto& scale = collider->get_current_local_scale();
    const auto img_height = scale[1] * 2.0f;
    text_font->compute_text_widths(t, img_height, text_widths);
    const auto raw_img_width = scale[0] * 2.0f;
    const auto is_bigger = raw_img_width < text_widths.back();
    const auto img_width = is_bigger ? raw_img_width : text_widths.back();
    {
        auto* const tran = cursor_model->get_transformation();
        math::Vec3 loc = tran->get_location();
        loc[0] = img_width * 0.5f;
        tran->set_location(loc);
    }
    core::sync::EndCaller<texture::Texture2D> txtend([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        text_material->set_color(txt);
        hint_text_model->set_enabled(false);
        text_model->set_enabled(true);
    });
    const auto _txt = text_font->bake(
        text, text_widths, theme.text_color,
        img_width, img_height, 0.0f, txtend);
    text_model->get_transformation()->local_x_scale(
        img_width / (text_model->get_collider()->get_current_local_scale()[0] * 2.0f));
}

void gearoenix::render::widget::Edit::set_hint_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    if (t.empty())
        return;
    hint_text = t;
    const auto& scale = collider->get_current_local_scale();
    const auto img_height = scale[1] * 2.0f;
    text_font->compute_text_widths(t, img_height, hint_text_widths);
    const auto raw_img_width = scale[0] * 2.0f;
    const auto is_bigger = raw_img_width < hint_text_widths.back();
    const auto img_width = is_bigger ? raw_img_width : hint_text_widths.back();
    core::sync::EndCaller<texture::Texture2D> txtend([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        hint_text_material->set_color(txt);
        text_model->set_enabled(false);
        hint_text_model->set_enabled(true);
    });
    const auto _txt = text_font->bake(
        hint_text, hint_text_widths, theme.hint_text_color,
        img_width, img_height, 0.0f, txtend);
    hint_text_model->get_transformation()->local_x_scale(
        img_width / (hint_text_model->get_collider()->get_current_local_scale()[0] * 2.0f));
}

void gearoenix::render::widget::Edit::active(bool b) noexcept
{
    cursor_model->set_enabled(b);
    actived = b;
}

bool gearoenix::render::widget::Edit::on_event(const core::event::Data& d) noexcept
{

    switch (d.source) {
    case core::event::Id::ButtonKeyboard: {
        if (actived) {
            const auto& data = std::get<gearoenix::core::event::button::KeyboardData>(d.data);
            if (data.action == core::event::button::KeyboardActionId::Press) {
                set_text(text + core::String::to_character(data.key).value());
            }
        }
        break;
    }
    default:
        GXUNEXPECTED
    }
    return false;
}
