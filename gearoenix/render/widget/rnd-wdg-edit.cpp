#include "rnd-wdg-edit.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-string.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/animation/phs-anm-manager.hpp"
#include "../../physics/collider/phs-cld-aabb.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../font/rnd-fnt-2d.hpp"
#include "../font/rnd-fnt-manager.hpp"
#include "../material/rnd-mat-unlit.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-dynamic.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include <cmath>
#include <string>

#define GX_EDIT_INIT Widget(my_id, Type::Edit, e, c), event_engine(e->get_system_application()->get_event_engine()), text_material(new material::Unlit(e, c)), hint_text_material(new material::Unlit(e, c)), background_material(new material::Unlit(e, c)), cursor_material(new material::Unlit(e, c))

void gearoenix::render::widget::Edit::init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& end_call) noexcept
{
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c([end_call, this] {
        event_engine->add_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
    });

    set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0f, 1.0f, 0.001f), math::Vec3(-1.0f, -1.0f, -0.001f)));

    auto* const ast_mgr = e->get_system_application()->get_asset_manager();
    auto* const msh_mgr = ast_mgr->get_mesh_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    auto* const fnt_mgr = ast_mgr->get_font_manager();

    core::sync::EndCaller<mesh::Mesh> msh_end([c](const std::shared_ptr<mesh::Mesh>&) {});
    const auto plate_mesh = msh_mgr->create_plate(msh_end);

    core::sync::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) {});
    text_font = fnt_mgr->get_default_2d(fend);

    text_material->set_translucency(material::TranslucencyMode::Transparent);
    hint_text_material->set_translucency(material::TranslucencyMode::Transparent);

    core::sync::EndCaller<model::Dynamic> mdl_end([c](const std::shared_ptr<model::Dynamic>&) {});

    text_model = mdl_mgr->create<model::Dynamic>(mdl_end);
    text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, text_material));
    auto* const text_tran = text_model->get_transformation();
    text_tran->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    text_tran->local_scale(theme.text_size);
    text_model->set_enabled(false);
    add_child(text_model);

    hint_text_model = mdl_mgr->create<model::Dynamic>(mdl_end);
    hint_text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, hint_text_material));
    auto* const hint_text_tran = hint_text_model->get_transformation();
    hint_text_tran->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    hint_text_tran->local_scale(theme.hint_text_size);
    hint_text_model->set_enabled(false);
    add_child(hint_text_model);

    background_material->set_color(theme.background_color, c);
    background_model = mdl_mgr->create<model::Dynamic>(mdl_end);
    background_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, background_material));
    add_child(background_model);

    cursor_material->set_color(theme.cursor_color, c);
    cursor_material->set_translucency(material::TranslucencyMode::Transparent);
    cursor_model = mdl_mgr->create<model::Dynamic>(mdl_end);
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

    on_scale();
}

void gearoenix::render::widget::Edit::on_scale() noexcept
{
    aspects[0] = (collider->get_current_local_scale()[0] - (collider->get_current_local_scale()[1] * (1.0f - theme.text_size))) * 2.0f;
    aspects[1] = collider->get_current_local_scale()[1] * theme.text_size * 2.0f;
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

void gearoenix::render::widget::Edit::compute_starting() noexcept
{
    text_font->compute_text_widths(text, aspects[1], text_widths);
    cursor_pos_in_text = text_widths[left_text.size()];
    starting_text_cut = 0.0f;
    ending_text_cut = text_widths[text.size()];
    if (ending_text_cut > aspects[0]) {
        if (left_to_right) {
            starting_text_cut = cursor_pos_in_text - aspects[0];
            ending_text_cut = cursor_pos_in_text;
        } else {
            starting_text_cut = cursor_pos_in_text;
            ending_text_cut = cursor_pos_in_text + aspects[0];
        }
    }
}

void gearoenix::render::widget::Edit::compute_cuts() noexcept
{
    starting_text_cut = GX_MAX(starting_text_cut, 0.0f);
    ending_text_cut = starting_text_cut + aspects[0];
    ending_text_cut = GX_MIN(ending_text_cut, text_widths[text.size()]);
}

void gearoenix::render::widget::Edit::refill_text() noexcept
{
    text.clear();
    for (const wchar_t ch : left_text) {
        text.push_back(ch);
    }
    for (auto i = right_text.rbegin(); i != right_text.rend(); ++i) {
        text.push_back(*i);
    }
}

void gearoenix::render::widget::Edit::refill_text_widths() noexcept
{
    text_font->compute_text_widths(text, aspects[1], text_widths);
}

void gearoenix::render::widget::Edit::place_cursor() noexcept
{
    const auto text_center = text_model->get_transformation()->get_location();
    const auto text_aspect = text_model->get_collider()->get_current_local_scale().xy();
    auto* const tran = cursor_model->get_transformation();
    math::Vec3 loc = tran->get_location();
    loc[0] = ((cursor_pos_in_text - starting_text_cut) - text_aspect[0]) + text_center[0];
    tran->set_location(loc);
}

void gearoenix::render::widget::Edit::render_text(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const auto img_width = std::abs(ending_text_cut - starting_text_cut) * 1.0001f;
    core::sync::EndCaller<texture::Texture2D> txt_end([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        text_material->set_color(txt);
        if (hint_text_model->get_enabled())
            hint_text_model->set_enabled(false);
        if (!text_model->get_enabled())
            text_model->set_enabled(true);
    });
    const auto _txt = text_font->bake(
        text, text_widths, theme.text_color,
        img_width, aspects[1], starting_text_cut, txt_end);
    text_model->get_transformation()->local_x_scale(
        img_width / (text_model->get_collider()->get_current_local_scale()[0] * 2.0f));
}

void gearoenix::render::widget::Edit::remove(const bool from_left, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const auto before = text_widths.back();
    refill_text();
    if (text.empty()) {
        clear();
    } else {
        refill_text_widths();
        cursor_pos_in_text = text_widths[left_text.size()];
        const auto margin = aspects[0] * 0.2f;
        const auto increase = text_widths.back() - before;
        if (from_left) {
            if (ending_text_cut >= text_widths.back()) {
                starting_text_cut += increase;
            } else if (starting_text_cut > 0.0 && cursor_pos_in_text - starting_text_cut < margin) {
                starting_text_cut = cursor_pos_in_text - margin;
            }
        } else {
            if (ending_text_cut < text_widths.back() && ending_text_cut - cursor_pos_in_text < margin) {
                starting_text_cut = (cursor_pos_in_text + margin) - aspects[0];
            } else if (starting_text_cut > 0.0f) {
                starting_text_cut += increase;
            }
        }
        compute_cuts();
        render_text(c);
        place_cursor();
    }
}

gearoenix::render::widget::Edit::Edit(
    const core::Id my_id,
    system::stream::Stream* const,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : GX_EDIT_INIT {
        GXUNIMPLEMENTED
    }

    gearoenix::render::widget::Edit::Edit(const core::Id my_id, engine::Engine* const e, const EditTheme& theme, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
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
    if (text != t) {
        left_text.clear();
        for (const wchar_t ch : t) {
            left_text.push_back(ch);
        }
        right_text.clear();
        temporary_left = 0;
        temporary_right = 0;
        text = t;
    }
    compute_starting();
    render_text(c);
    place_cursor();
}

void gearoenix::render::widget::Edit::set_hint_text(
    const std::wstring& t,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    if (t.empty())
        return;
    hint_text = t;
    const auto& scale = collider->get_current_local_scale();
    const auto img_height = scale[1] * 2.0f * theme.hint_text_size;
    text_font->compute_text_widths(t, img_height, hint_text_widths);
    const auto raw_img_width = (scale[0] - scale[1] * (1.0f - theme.hint_text_size)) * 2.0f;
    const auto is_bigger = raw_img_width < hint_text_widths.back();
    const auto img_width = is_bigger ? raw_img_width : hint_text_widths.back();
    core::sync::EndCaller<texture::Texture2D> txt_end([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
        hint_text_material->set_color(txt);
        text_model->set_enabled(false);
        hint_text_model->set_enabled(true);
    });
    const auto _txt = text_font->bake(
        hint_text, hint_text_widths, theme.hint_text_color,
        img_width, img_height, 0.0f, txt_end);
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
            const auto& data = std::get<core::event::button::KeyboardData>(d.data);
            if (data.action == core::event::button::KeyboardActionId::Press) {
                const auto pressed_count = event_engine->get_pressed_keyboard_buttons().size();
                const bool shift_pressed = event_engine->is_pressed(core::event::button::KeyboardKeyId::LeftShift) || event_engine->is_pressed(core::event::button::KeyboardKeyId::RightShift);
                const auto key = core::String::to_character(data.key, shift_pressed);
                if (key.has_value()) {
                    if (pressed_count == 2 && (event_engine->is_pressed(core::event::button::KeyboardKeyId::LeftControl) || event_engine->is_pressed(core::event::button::KeyboardKeyId::RightControl)) && event_engine->is_pressed(core::event::button::KeyboardKeyId::V)) {
                        const char* const clipboard = e->get_system_application()->get_clipboard();
                        if (clipboard != nullptr) {
                            for (int i = 0; clipboard[i] != 0; ++i) {
                                insert(clipboard[i]);
                            }
                        }
                    } else {
                        insert(key.value());
                    }
                } else if (pressed_count == 1) {
                    if (data.key == core::event::button::KeyboardKeyId::Left) {
                        if (!text.empty()) {
                            temporary_right = 0;
                            temporary_left = 0;
                            if (!left_text.empty()) {
                                right_text.push_back(left_text.back());
                                left_text.pop_back();
                            }
                            const auto move = cursor_pos_in_text - starting_text_cut;
                            cursor_pos_in_text = text_widths[left_text.size()];
                            if (starting_text_cut > 0.0 && cursor_pos_in_text - starting_text_cut < aspects[0] * 0.2f) {
                                starting_text_cut = cursor_pos_in_text - move;
                                compute_cuts();
                                render_text();
                            }
                            place_cursor();
                        }
                    } else if (data.key == core::event::button::KeyboardKeyId::Right) {
                        if (!text.empty()) {
                            temporary_right = 0;
                            temporary_left = 0;
                            if (!right_text.empty()) {
                                left_text.push_back(right_text.back());
                                right_text.pop_back();
                            }
                            const auto move = cursor_pos_in_text - starting_text_cut;
                            cursor_pos_in_text = text_widths[left_text.size()];
                            if (ending_text_cut < text_widths[text.size()] && ending_text_cut - cursor_pos_in_text < aspects[0] * 0.2f) {
                                starting_text_cut = cursor_pos_in_text - move;
                                compute_cuts();
                                render_text();
                            }
                            place_cursor();
                        }
                    } else if (data.key == core::event::button::KeyboardKeyId::Backspace) {
                        backspace();
                    } else if (data.key == core::event::button::KeyboardKeyId::Delete) {
                        del();
                    }
                }
            }
        }
        break;
    }
    default:
        GXUNEXPECTED
    }
    return false;
}

void gearoenix::render::widget::Edit::set_left_to_right(const bool b) noexcept
{
    if (left_to_right == b)
        return;
    if (left_to_right) {
        for (int i = static_cast<int>(left_text.size()) - 1; i >= 0; --i) {
            right_text.push_back(left_text[i]);
        }
        left_text.clear();
        text.clear();
        for (int i = static_cast<int>(right_text.size()) - 1; i >= 0; --i) {
            text.push_back(right_text[i]);
        }
    } else {
        for (int i = static_cast<int>(right_text.size()) - 1; i >= 0; --i) {
            left_text.push_back(right_text[i]);
        }
        right_text.clear();
        text.clear();
        for (const wchar_t c : left_text) {
            text.push_back(c);
        }
    }
    left_to_right = b;
    temporary_left = 0;
    temporary_right = 0;
}

void gearoenix::render::widget::Edit::insert(
    const wchar_t character,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    bool increase_to_right = true;
    if ((static_cast<int>(character) > 31 && static_cast<int>(character) < 127) || (static_cast<int>(character) > 1631 && static_cast<int>(character) < 1642)) {
        if (left_to_right) {
            if (temporary_right > 0) {
                for (std::size_t i = 0; i < temporary_right; ++i) {
                    left_text.push_back(right_text.back());
                    right_text.pop_back();
                }
                right_text.clear();
                temporary_right = 0;
            }
        } else {
            ++temporary_left;
        }
        left_text.push_back(character);
    } else if ((static_cast<int>(character) > 1535 && static_cast<int>(character) < 1791) || (static_cast<int>(character) > 1871 && static_cast<int>(character) < 1919) || (static_cast<int>(character) > 64335 && static_cast<int>(character) < 65023) || (static_cast<int>(character) > 65135 && static_cast<int>(character) < 65279)) {
        if (left_to_right) {
            ++temporary_right;
        } else {
            if (temporary_left > 0) {
                for (std::size_t i = 0; i < temporary_left; ++i) {
                    right_text.push_back(left_text.back());
                    left_text.pop_back();
                }
                left_text.clear();
                temporary_left = 0;
            }
        }
        right_text.push_back(character);
        increase_to_right = false;
    } else {
        return;
    }
    const core::Real cursor_before = increase_to_right ? cursor_pos_in_text - starting_text_cut : ending_text_cut - cursor_pos_in_text;
    const core::Real before = text_widths.empty() ? 0.0f : text_widths[text.size()];
    refill_text();
    text_font->compute_text_widths(text, aspects[1], text_widths);
    cursor_pos_in_text = text_widths[left_text.size()];
    const core::Real increase = text_widths[text.size()] - before;
    if (increase_to_right) {
        if (cursor_before + increase > aspects[0]) {
            ending_text_cut = cursor_pos_in_text;
            starting_text_cut = cursor_pos_in_text - aspects[0];
        } else if (text_widths[text.size()] < aspects[0]) {
            starting_text_cut = 0.0f;
            ending_text_cut = text_widths[text.size()];
        }
    } else {
        if (cursor_before - increase < 0.0f) {
            starting_text_cut = cursor_pos_in_text;
            ending_text_cut = cursor_pos_in_text + aspects[0];
        } else if (text_widths[text.size()] < aspects[0]) {
            starting_text_cut = 0.0f;
            ending_text_cut = text_widths[text.size()];
        }
    }
    render_text(c);
    place_cursor();
}

void gearoenix::render::widget::Edit::selected(const gearoenix::math::Vec3& point) noexcept
{
    if (text.empty())
        return;
    auto* const text_tran = text_model->get_transformation();
    const auto v = point - text_tran->get_location();
    const auto x = v.dot(text_tran->get_x_axis());
    const auto pos_in_txt = x + text_model->get_collider()->get_current_local_scale()[0] + starting_text_cut;
    auto min_dis = (aspects[0] + text_widths[text.size()]) * 10.0f;
    int index;
    for (index = 0; index < static_cast<int>(text_widths.size()); ++index) {
        auto dis = std::abs(pos_in_txt - text_widths[index]);
        if (dis > min_dis) {
            --index;
            break;
        }
        min_dis = dis;
    }
    if (index > static_cast<int>(text.size())) {
        index = text.size();
    } else if (index < 0) {
        index = 0;
    }
    left_text.clear();
    right_text.clear();
    for (int i = 0; i < index; ++i) {
        left_text.push_back(text[i]);
    }
    const int end = static_cast<int>(text.size() - 1);
    for (int i = end; i >= static_cast<int>(index); --i) {
        right_text.push_back(text[i]);
    }
    cursor_pos_in_text = text_widths[left_text.size()];
    temporary_left = 0;
    temporary_right = 0;
    place_cursor();
}

void gearoenix::render::widget::Edit::backspace(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    if (text.empty())
        return;
    bool removed_from_left = true;
    if (left_to_right) {
        if (temporary_right > 0) {
            --temporary_right;
            right_text.pop_back();
            removed_from_left = false;
        } else {
            if (left_text.empty())
                return;
            left_text.pop_back();
        }
    } else {
        if (temporary_left > 0) {
            --temporary_left;
            left_text.pop_back();
        } else {
            if (right_text.empty())
                return;
            removed_from_left = false;
            right_text.pop_back();
        }
    }
    remove(removed_from_left, c);
}

void gearoenix::render::widget::Edit::del(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    if (text.empty())
        return;
    if (left_to_right) {
        if (temporary_right > 0) {
            if (right_text.size() > temporary_right) {
                for (auto i = right_text.size() - (temporary_right + 1); i < right_text.size();) {
                    auto ii = i + 1;
                    right_text[i] = right_text[ii];
                    i = ii;
                }
                right_text.pop_back();
            } else {
                return;
            }
        } else {
            if (right_text.empty())
                return;
            right_text.pop_back();
        }
    } else {
        if (temporary_left > 0) {
            if (left_text.size() > temporary_left) {
                for (auto i = left_text.size() - (temporary_left + 1); i < left_text.size();) {
                    auto ii = i + 1;
                    left_text[i] = left_text[ii];
                    i = ii;
                }
                left_text.pop_back();
            } else {
                return;
            }
        } else {
            if (right_text.empty())
                return;
            right_text.pop_back();
        }
    }
    remove(!left_to_right, c);
}

void gearoenix::render::widget::Edit::clear() noexcept
{
    cursor_pos_in_text = 0.0f;
    starting_text_cut = 0.0f;
    ending_text_cut = 0.0f;
    text_model->set_enabled(false);
    hint_text_model->set_enabled(true);
    auto* const tran = text_model->get_transformation();
    cursor_model->get_transformation()->set_location(tran->get_location() + tran->get_z_axis() * 0.02f);
}
