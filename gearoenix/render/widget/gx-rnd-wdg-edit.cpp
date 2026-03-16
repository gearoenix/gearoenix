// #include "gx-rnd-wdg-edit.hpp"
// #include "../../core/asset/gx-cr-asset-manager.hpp"
// #include "../../core/event/gx-cr-ev-engine.hpp"
// #include "../../core/gx-cr-string.hpp"
// #include "../../physics/animation/gx-phs-anm-animation.hpp"
// #include "../../physics/animation/gx-phs-anm-manager.hpp"
// #include "../../physics/collider/gx-phs-cld-aabb.hpp"
// #include "../../physics/gx-phs-engine.hpp"
// #include "../../platform/gx-plt-application.hpp"
// #include "../engine/gx-rnd-eng-engine.hpp"
// #include "../font/gx-rnd-fnt-font.hpp"
// #include "../font/gx-rnd-fnt-manager.hpp"
// #include "../material/gx-rnd-mat-unlit.hpp"
// #include "../mesh/gx-rnd-msh-builder.hpp"
// #include "../mesh/gx-rnd-msh-manager.hpp"
// #include "../model/gx-rnd-mdl-dynamic.hpp"
// #include "../model/gx-rnd-mdl-manager.hpp"
// #include "../model/gx-rnd-mdl-mesh.hpp"
// #include <cmath>
// #include <string>
//
// #define GX_EDIT_INIT                                                     \
//     Widget(my_id, std::move(name), Type::Edit, e, c),                    \
//         event_engine(e->get_platform_application()->get_event_engine()), \
//         text_material(new material::Unlit(e, c)),                        \
//         hint_text_material(new material::Unlit(e, c)),                   \
//         background_material(new material::Unlit(e, c)),                  \
//         cursor_material(new material::Unlit(e, c))
//
// void gearoenix::render::widget::Edit::init(const core::job::EndCaller<core::job::EndCallerIgnore>& end_call)
// {
//     const core::job::EndCaller<core::job::EndCallerIgnore> c([end_call, this] {
//         event_engine->add_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
//     });
//
//     set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0, 1.0, math::Numeric::epsilon), math::Vec3(-1.0, -1.0, -math::Numeric::epsilon)));
//
//     auto* const ast_mgr = e->get_platform_application()->get_asset_manager();
//     auto* const msh_mgr = ast_mgr->get_mesh_manager();
//     auto* const mdl_mgr = ast_mgr->get_model_manager();
//     auto* const fnt_mgr = ast_mgr->get_font_manager();
//
//     core::job::EndCaller<mesh::Mesh> msh_end([c](const std::shared_ptr<mesh::Mesh>&) { });
//     const auto plate_mesh = msh_mgr->create_plate(msh_end);
//
//     core::job::EndCaller<font::Font> fend([c](const std::shared_ptr<font::Font>&) { });
//     text_font = fnt_mgr->get_default_2d(fend);
//
//     text_material->set_translucency(material::TranslucencyMode::Transparent);
//     hint_text_material->set_translucency(material::TranslucencyMode::Transparent);
//
//     core::job::EndCaller<model::Dynamic> mdl_end([c](const std::shared_ptr<model::Dynamic>&) { });
//
//     text_model = mdl_mgr->create<model::Dynamic>("gx-edit-" + name + "-txt", mdl_end);
//     text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, text_material));
//     auto* const text_tran = text_model->get_transformation();
//     text_tran->set_location(math::Vec3(0.0, 0.0, 0.01));
//     text_tran->local_scale(theme.text_size);
//     text_model->set_enabled(false);
//     add_child(text_model);
//
//     hint_text_model = mdl_mgr->create<model::Dynamic>("gx-edit-" + name + "-hint-txt", mdl_end);
//     hint_text_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, hint_text_material));
//     auto* const hint_text_tran = hint_text_model->get_transformation();
//     hint_text_tran->set_location(math::Vec3(0.0, 0.0, 0.01));
//     hint_text_tran->local_scale(theme.hint_text_size);
//     hint_text_model->set_enabled(false);
//     add_child(hint_text_model);
//
//     background_material->set_color(math::Vec4<float>(theme.background_color), c);
//     background_model = mdl_mgr->create<model::Dynamic>("gx-edit-" + name + "-bg", mdl_end);
//     background_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, background_material));
//     add_child(background_model);
//
//     cursor_material->set_color(math::Vec4<float>(theme.cursor_color), c);
//     cursor_material->set_translucency(material::TranslucencyMode::Transparent);
//     cursor_model = mdl_mgr->create<model::Dynamic>("gx-edit-" + name + "-crs", mdl_end);
//     cursor_model->add_mesh(std::make_shared<model::Mesh>(plate_mesh, cursor_material));
//     auto* const cursor_tran = cursor_model->get_transformation();
//     cursor_tran->set_location(math::Vec3(0.0, 0.0, 0.02));
//     cursor_tran->local_scale(theme.cursor_size);
//     add_child(cursor_model);
//
//     const auto t1 = theme.cursor_state_time;
//     const auto t2 = theme.cursor_blink_time;
//     const auto t3 = theme.cursor_state_time + theme.cursor_blink_time;
//     const auto t4 = t3 + theme.cursor_state_time;
//     const auto t5 = t4 + theme.cursor_blink_time;
//
//     cursor_animation = std::make_shared<physics::animation::Animation>(
//         "gx-edit-" + name + "-crs-anm",
//         [cursor_material { cursor_material }, t1, t2, t3, t4, t5](
//             const double from_start, const double) {
//             const auto s = from_start - std::floor(from_start / t5) * t5;
//             if (t1 > s) {
//                 cursor_material->set_alpha(1.0f);
//             } else if (t3 > s) {
//                 cursor_material->set_alpha(static_cast<float>(1.0 - (s - t1) / t2));
//             } else if (t4 > s) {
//                 cursor_material->set_alpha(0.0f);
//             } else {
//                 cursor_material->set_alpha(static_cast<float>((s - t4) / t2));
//             }
//         },
//         std::numeric_limits<double>::max());
//     e->get_physics_engine()->get_animation_manager()->add(get_root()->get_id(), cursor_animation);
//
//     collider->set_on_scale([this] { on_scale(); });
//
//     on_scale();
// }
//
// void gearoenix::render::widget::Edit::on_scale()
// {
//     aspects[0] = (collider->get_current_local_scale()[0] - (collider->get_current_local_scale()[1] * (1.0f - theme.text_size))) * 2.0f;
//     aspects[1] = collider->get_current_local_scale()[1] * theme.text_size * 2.0f;
//     if (text.empty()) {
//         if (!hint_text.empty()) {
//             set_hint_text(text);
//         }
//     } else {
//         set_text(text);
//     }
//     auto cursor_scale = static_cast<double>(theme.cursor_width) * 0.5f;
//     cursor_scale /= static_cast<double>(e->get_platform_application()->get_event_engine()->get_window_width());
//     cursor_scale /= cursor_model->get_collider()->get_current_local_scale()[0];
//     cursor_model->get_transformation()->local_x_scale(cursor_scale);
// }
//
// void gearoenix::render::widget::Edit::compute_starting()
// {
//     text_font->compute_text_widths(text, aspects[1], text_widths);
//     cursor_pos_in_text = text_widths[left_text.size()];
//     starting_text_cut = 0.0f;
//     ending_text_cut = text_widths[text.size()];
//     if (ending_text_cut > aspects[0]) {
//         if (left_to_right) {
//             starting_text_cut = cursor_pos_in_text - aspects[0];
//             ending_text_cut = cursor_pos_in_text;
//         } else {
//             starting_text_cut = cursor_pos_in_text;
//             ending_text_cut = cursor_pos_in_text + aspects[0];
//         }
//     }
// }
//
// void gearoenix::render::widget::Edit::compute_cuts()
// {
//     starting_text_cut = GX_MAX(starting_text_cut, 0.0f);
//     ending_text_cut = starting_text_cut + aspects[0];
//     ending_text_cut = GX_MIN(ending_text_cut, text_widths[text.size()]);
// }
//
// void gearoenix::render::widget::Edit::refill_text()
// {
//     text.clear();
//     for (const wchar_t ch : left_text) {
//         text.push_back(ch);
//     }
//     for (auto i = right_text.rbegin(); i != right_text.rend(); ++i) {
//         text.push_back(*i);
//     }
// }
//
// void gearoenix::render::widget::Edit::refill_text_widths()
// {
//     text_font->compute_text_widths(text, aspects[1], text_widths);
// }
//
// void gearoenix::render::widget::Edit::place_cursor()
// {
//     const auto text_center = text_model->get_transformation()->get_location();
//     const auto text_aspect = text_model->get_collider()->get_current_local_scale().xy();
//     auto* const tran = cursor_model->get_transformation();
//     math::Vec3 loc = tran->get_location();
//     loc[0] = ((cursor_pos_in_text - starting_text_cut) - text_aspect[0]) + text_center[0];
//     tran->set_location(loc);
// }
//
// void gearoenix::render::widget::Edit::render_text(const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     const auto img_width = std::abs(ending_text_cut - starting_text_cut) * 1.0001f;
//     core::job::EndCaller<texture::Texture2D> txt_end([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
//         text_material->set_color(txt);
//         if (hint_text_model->get_enabled())
//             hint_text_model->set_enabled(false);
//         if (!text_model->get_enabled())
//             text_model->set_enabled(true);
//     });
//     const auto _txt = text_font->bake(
//         text, text_widths, theme.text_color,
//         img_width, aspects[1], starting_text_cut, txt_end);
//     text_model->get_transformation()->local_x_scale(
//         img_width / (text_model->get_collider()->get_current_local_scale()[0] * 2.0f));
// }
//
// void gearoenix::render::widget::Edit::remove(const bool from_left, const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     const auto before = text_widths.back();
//     refill_text();
//     if (text.empty()) {
//         clear();
//     } else {
//         refill_text_widths();
//         cursor_pos_in_text = text_widths[left_text.size()];
//         const auto margin = aspects[0] * 0.2f;
//         const auto increase = text_widths.back() - before;
//         if (from_left) {
//             if (ending_text_cut >= text_widths.back()) {
//                 starting_text_cut += increase;
//             } else if (starting_text_cut > 0.0 && cursor_pos_in_text - starting_text_cut < margin) {
//                 starting_text_cut = cursor_pos_in_text - margin;
//             }
//         } else {
//             if (ending_text_cut < text_widths.back() && ending_text_cut - cursor_pos_in_text < margin) {
//                 starting_text_cut = (cursor_pos_in_text + margin) - aspects[0];
//             } else if (starting_text_cut > 0.0f) {
//                 starting_text_cut += increase;
//             }
//         }
//         compute_cuts();
//         render_text(c);
//         place_cursor();
//     }
// }
//
// gearoenix::render::widget::Edit::Edit(
//     const core::Id my_id,
//     std::string name,
//     platform::stream::Stream* const,
//     engine::Engine* const e,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
//     : GX_EDIT_INIT {
//         GX_UNIMPLEMENTED
//     }
//
//     gearoenix::render::widget::Edit::Edit(const core::Id my_id, std::string name, engine::Engine* const e, const EditTheme& theme, const core::job::EndCaller<core::job::EndCallerIgnore>& c)
//     : GX_EDIT_INIT
//     , theme(theme)
// {
//     init(c);
// }
//
// gearoenix::render::widget::Edit::Edit(
//     const core::Id my_id,
//     std::string name,
//     engine::Engine* const e,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
//     : GX_EDIT_INIT
// {
//     init(c);
// }
//
// #define GX_EDIT_CONS(...)                              \
//     std::shared_ptr<Edit> self(new Edit(__VA_ARGS__)); \
//     self->model_self = self;                           \
//     self->widget_self = self;                          \
//     self->edit_self = self;                            \
//     return self
//
// std::shared_ptr<gearoenix::render::widget::Edit> gearoenix::render::widget::Edit::construct(
//     const core::Id id,
//     std::string name,
//     platform::stream::Stream* const f,
//     engine::Engine* const e,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_EDIT_CONS(id, std::move(name), f, e, c);
// }
//
// std::shared_ptr<gearoenix::render::widget::Edit> gearoenix::render::widget::Edit::construct(
//     const core::Id id,
//     std::string name,
//     engine::Engine* const e,
//     const EditTheme& theme,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_EDIT_CONS(id, std::move(name), e, theme, c);
// }
//
// std::shared_ptr<gearoenix::render::widget::Edit> gearoenix::render::widget::Edit::construct(
//     const core::Id id,
//     std::string name,
//     engine::Engine* const e,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_EDIT_CONS(id, std::move(name), e, c);
// }
//
// gearoenix::render::widget::Edit::~Edit()
// {
//     cursor_animation->set_activity(false);
// }
//
// void gearoenix::render::widget::Edit::set_text(
//     const std::wstring& t,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_GUARD_LOCK(text)
//     if (t.empty())
//         return;
//     if (text != t) {
//         left_text.clear();
//         for (const wchar_t ch : t) {
//             left_text.push_back(ch);
//         }
//         right_text.clear();
//         temporary_left = 0;
//         temporary_right = 0;
//         text = t;
//     }
//     compute_starting();
//     render_text(c);
//     place_cursor();
// }
//
// void gearoenix::render::widget::Edit::set_hint_text(
//     const std::wstring& t,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_GUARD_LOCK(text)
//     if (t.empty())
//         return;
//     hint_text = t;
//     const auto& scale = collider->get_current_local_scale();
//     const auto img_height = scale[1] * 2.0f * theme.hint_text_size;
//     text_font->compute_text_widths(t, img_height, hint_text_widths);
//     const auto raw_img_width = (scale[0] - scale[1] * (1.0f - theme.hint_text_size)) * 2.0f;
//     const auto is_bigger = raw_img_width < hint_text_widths.back();
//     const auto img_width = is_bigger ? raw_img_width : hint_text_widths.back();
//     core::job::EndCaller<texture::Texture2D> txt_end([c, this](const std::shared_ptr<texture::Texture2D>& txt) {
//         hint_text_material->set_color(txt);
//         text_model->set_enabled(false);
//         hint_text_model->set_enabled(true);
//     });
//     const auto _txt = text_font->bake(
//         hint_text, hint_text_widths, theme.hint_text_color,
//         img_width, img_height, 0.0f, txt_end);
//     hint_text_model->get_transformation()->local_x_scale(
//         img_width / (hint_text_model->get_collider()->get_current_local_scale()[0] * 2.0f));
// }
//
// void gearoenix::render::widget::Edit::active(bool b)
// {
//     cursor_model->set_enabled(b);
//     actived = b;
// }
//
// bool gearoenix::render::widget::Edit::on_event(const core::event::Data& d)
// {
//     switch (d.get_source()) {
//     case core::event::Id::ButtonKeyboard: {
//         if (actived) {
//             const auto& data = std::get<core::event::button::KeyboardData>(d.get_data());
//             if (data.get_action() == core::event::button::KeyboardActionId::Press) {
//                 const auto pressed_count = event_engine->get_pressed_keyboard_buttons().size();
//                 const bool shift_pressed = event_engine->is_pressed(core::event::button::KeyboardKeyId::LeftShift) || event_engine->is_pressed(core::event::button::KeyboardKeyId::RightShift);
//                 const auto key = core::String::to_character(data.get_key(), shift_pressed);
//                 if (key.has_value()) {
//                     if (pressed_count == 2 && (event_engine->is_pressed(core::event::button::KeyboardKeyId::LeftControl) || event_engine->is_pressed(core::event::button::KeyboardKeyId::RightControl)) &&
//                     event_engine->is_pressed(core::event::button::KeyboardKeyId::V)) {
//                         const char* const clipboard = e->get_platform_application()->get_clipboard();
//                         if (clipboard != nullptr) {
//                             for (int i = 0; clipboard[i] != 0; ++i) {
//                                 insert(clipboard[i]);
//                             }
//                         }
//                     } else {
//                         insert(key.value());
//                     }
//                 } else if (pressed_count == 1) {
//                     if (data.get_key() == core::event::button::KeyboardKeyId::Left) {
//                         if (!text.empty()) {
//                             temporary_right = 0;
//                             temporary_left = 0;
//                             if (!left_text.empty()) {
//                                 right_text.push_back(left_text.back());
//                                 left_text.pop_back();
//                             }
//                             const auto move = cursor_pos_in_text - starting_text_cut;
//                             cursor_pos_in_text = text_widths[left_text.size()];
//                             if (starting_text_cut > 0.0 && cursor_pos_in_text - starting_text_cut < aspects[0] * 0.2f) {
//                                 starting_text_cut = cursor_pos_in_text - move;
//                                 compute_cuts();
//                                 render_text();
//                             }
//                             place_cursor();
//                         }
//                     } else if (data.get_key() == core::event::button::KeyboardKeyId::Right) {
//                         if (!text.empty()) {
//                             temporary_right = 0;
//                             temporary_left = 0;
//                             if (!right_text.empty()) {
//                                 left_text.push_back(right_text.back());
//                                 right_text.pop_back();
//                             }
//                             const auto move = cursor_pos_in_text - starting_text_cut;
//                             cursor_pos_in_text = text_widths[left_text.size()];
//                             if (ending_text_cut < text_widths[text.size()] && ending_text_cut - cursor_pos_in_text < aspects[0] * 0.2f) {
//                                 starting_text_cut = cursor_pos_in_text - move;
//                                 compute_cuts();
//                                 render_text();
//                             }
//                             place_cursor();
//                         }
//                     } else if (data.get_key() == core::event::button::KeyboardKeyId::Backspace) {
//                         backspace();
//                     } else if (data.get_key() == core::event::button::KeyboardKeyId::Delete) {
//                         del();
//                     }
//                 }
//             }
//         }
//         break;
//     }
//     default:
//         GX_UNEXPECTED
//     }
//     return false;
// }
//
// void gearoenix::render::widget::Edit::set_left_to_right(const bool b)
// {
//     GX_GUARD_LOCK(text)
//     if (left_to_right == b)
//         return;
//     if (left_to_right) {
//         for (int i = static_cast<int>(left_text.size()) - 1; i >= 0; --i) {
//             right_text.push_back(left_text[i]);
//         }
//         left_text.clear();
//         text.clear();
//         for (int i = static_cast<int>(right_text.size()) - 1; i >= 0; --i) {
//             text.push_back(right_text[i]);
//         }
//     } else {
//         for (int i = static_cast<int>(right_text.size()) - 1; i >= 0; --i) {
//             left_text.push_back(right_text[i]);
//         }
//         right_text.clear();
//         text.clear();
//         for (const wchar_t c : left_text) {
//             text.push_back(c);
//         }
//     }
//     left_to_right = b;
//     temporary_left = 0;
//     temporary_right = 0;
// }
//
// void gearoenix::render::widget::Edit::insert(
//     const wchar_t character,
//     const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_GUARD_LOCK(text)
//     bool increase_to_right = true;
//     if ((static_cast<int>(character) > 31 && static_cast<int>(character) < 127) || (static_cast<int>(character) > 1631 && static_cast<int>(character) < 1642)) {
//         if (left_to_right) {
//             if (temporary_right > 0) {
//                 for (std::uint64_t i = 0; i < temporary_right; ++i) {
//                     left_text.push_back(right_text.back());
//                     right_text.pop_back();
//                 }
//                 right_text.clear();
//                 temporary_right = 0;
//             }
//         } else {
//             ++temporary_left;
//         }
//         left_text.push_back(character);
//     } else if ((static_cast<int>(character) > 1535 && static_cast<int>(character) < 1791) || (static_cast<int>(character) > 1871 && static_cast<int>(character) < 1919) || (static_cast<int>(character) > 64335 && static_cast<int>(character) < 65023)
//     || (static_cast<int>(character) > 65135 && static_cast<int>(character) < 65279)) {
//         if (left_to_right) {
//             ++temporary_right;
//         } else {
//             if (temporary_left > 0) {
//                 for (std::uint64_t i = 0; i < temporary_left; ++i) {
//                     right_text.push_back(left_text.back());
//                     left_text.pop_back();
//                 }
//                 left_text.clear();
//                 temporary_left = 0;
//             }
//         }
//         right_text.push_back(character);
//         increase_to_right = false;
//     } else {
//         return;
//     }
//     const double cursor_before = increase_to_right ? cursor_pos_in_text - starting_text_cut : ending_text_cut - cursor_pos_in_text;
//     const double before = text_widths.empty() ? 0.0f : text_widths[text.size()];
//     refill_text();
//     text_font->compute_text_widths(text, aspects[1], text_widths);
//     cursor_pos_in_text = text_widths[left_text.size()];
//     const double increase = text_widths[text.size()] - before;
//     if (increase_to_right) {
//         if (cursor_before + increase > aspects[0]) {
//             ending_text_cut = cursor_pos_in_text;
//             starting_text_cut = cursor_pos_in_text - aspects[0];
//         } else if (text_widths[text.size()] < aspects[0]) {
//             starting_text_cut = 0.0f;
//             ending_text_cut = text_widths[text.size()];
//         }
//     } else {
//         if (cursor_before - increase < 0.0f) {
//             starting_text_cut = cursor_pos_in_text;
//             ending_text_cut = cursor_pos_in_text + aspects[0];
//         } else if (text_widths[text.size()] < aspects[0]) {
//             starting_text_cut = 0.0f;
//             ending_text_cut = text_widths[text.size()];
//         }
//     }
//     render_text(c);
//     place_cursor();
// }
//
// void gearoenix::render::widget::Edit::selected(const math::Vec3<double>& point)
// {
//     e->get_platform_application()->set_soft_keyboard_visibility(true);
//     if (text.empty())
//         return;
//     auto* const text_tran = text_model->get_transformation();
//     const auto v = point - text_tran->get_location();
//     const auto x = v.dot(text_tran->get_x_axis());
//     const auto pos_in_txt = x + text_model->get_collider()->get_current_local_scale()[0] + starting_text_cut;
//     auto min_dis = (aspects[0] + text_widths[text.size()]) * 10.0f;
//     int index;
//     for (index = 0; index < static_cast<int>(text_widths.size()); ++index) {
//         auto dis = std::abs(pos_in_txt - text_widths[index]);
//         if (dis > min_dis) {
//             --index;
//             break;
//         }
//         min_dis = dis;
//     }
//     if (index > static_cast<int>(text.size())) {
//         index = static_cast<int>(text.size());
//     } else if (index < 0) {
//         index = 0;
//     }
//     left_text.clear();
//     right_text.clear();
//     for (int i = 0; i < index; ++i) {
//         left_text.push_back(text[i]);
//     }
//     const int end = static_cast<int>(text.size() - 1);
//     for (int i = end; i >= static_cast<int>(index); --i) {
//         right_text.push_back(text[i]);
//     }
//     cursor_pos_in_text = text_widths[left_text.size()];
//     temporary_left = 0;
//     temporary_right = 0;
//     place_cursor();
// }
//
// void gearoenix::render::widget::Edit::backspace(const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_GUARD_LOCK(text)
//     if (text.empty())
//         return;
//     bool removed_from_left = true;
//     if (left_to_right) {
//         if (temporary_right > 0) {
//             --temporary_right;
//             right_text.pop_back();
//             removed_from_left = false;
//         } else {
//             if (left_text.empty())
//                 return;
//             left_text.pop_back();
//         }
//     } else {
//         if (temporary_left > 0) {
//             --temporary_left;
//             left_text.pop_back();
//         } else {
//             if (right_text.empty())
//                 return;
//             removed_from_left = false;
//             right_text.pop_back();
//         }
//     }
//     remove(removed_from_left, c);
// }
//
// void gearoenix::render::widget::Edit::del(const core::job::EndCaller<core::job::EndCallerIgnore>& c)
// {
//     GX_GUARD_LOCK(text)
//     if (text.empty())
//         return;
//     if (left_to_right) {
//         if (temporary_right > 0) {
//             if (right_text.size() > temporary_right) {
//                 for (auto i = right_text.size() - (temporary_right + 1); i < right_text.size();) {
//                     auto ii = i + 1;
//                     right_text[i] = right_text[ii];
//                     i = ii;
//                 }
//                 right_text.pop_back();
//             } else {
//                 return;
//             }
//         } else {
//             if (right_text.empty())
//                 return;
//             right_text.pop_back();
//         }
//     } else {
//         if (temporary_left > 0) {
//             if (left_text.size() > temporary_left) {
//                 for (auto i = left_text.size() - (temporary_left + 1); i < left_text.size();) {
//                     auto ii = i + 1;
//                     left_text[i] = left_text[ii];
//                     i = ii;
//                 }
//                 left_text.pop_back();
//             } else {
//                 return;
//             }
//         } else {
//             if (right_text.empty())
//                 return;
//             right_text.pop_back();
//         }
//     }
//     remove(!left_to_right, c);
// }
//
// void gearoenix::render::widget::Edit::clear()
// {
//     GX_GUARD_LOCK(text)
//     cursor_pos_in_text = 0.0f;
//     starting_text_cut = 0.0f;
//     ending_text_cut = 0.0f;
//     text_model->set_enabled(false);
//     hint_text_model->set_enabled(true);
//     auto* const tran = text_model->get_transformation();
//     cursor_model->get_transformation()->set_location(tran->get_location() + tran->get_z_axis() * 0.02f);
// }
