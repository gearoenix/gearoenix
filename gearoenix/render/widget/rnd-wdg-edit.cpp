#include "rnd-wdg-edit.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/animation/phs-anm-manager.hpp"
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

void gearoenix::render::widget::Edit::init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{

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
    hint_text_material->set_color(math::Vec4(0.0f), c);

    core::sync::EndCaller<model::Dynamic> mdlend([c](const std::shared_ptr<model::Dynamic>&) {});

    text_model = mdlmgr->create<model::Dynamic>(mdlend);
    text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, text_material));
    add_child(text_model);

    hint_text_model = mdlmgr->create<model::Dynamic>(mdlend);
    hint_text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, hint_text_material));
    hint_text_model->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
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
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Edit, f, e, c) {
        GXUNIMPLEMENTED
    }

    gearoenix::render::widget::Edit::Edit(const core::Id my_id, engine::Engine* const e, const Theme& theme, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Edit, e, c)
    , text_material(new material::Material(e, c))
    , hint_text_material(new material::Material(e, c))
    , background_material(new material::Material(e, c))
    , cursor_material(new material::Material(e, c))
    , theme(theme)
{
    init(c);
}

gearoenix::render::widget::Edit::Edit(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Edit, e, c)
    , text_material(new material::Material(e, c))
    , hint_text_material(new material::Material(e, c))
    , background_material(new material::Material(e, c))
    , cursor_material(new material::Material(e, c))
{
    init(c);
}

gearoenix::render::widget::Edit::~Edit() noexcept
{
    cursor_animation->set_activity(false);
}

void gearoenix::render::widget::Edit::set_text(
    const std::wstring&,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
{
    GXUNIMPLEMENTED
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
    });
    const auto _txt = text_font->bake(
        hint_text, hint_text_widths, theme.hint_text_color,
        img_width, img_height, 0.0f, txtend);
    hint_text_model->get_transformation()->local_x_scale(
        img_width / (hint_text_model->get_collider()->get_current_local_scale()[0] * 2.0f));
}
