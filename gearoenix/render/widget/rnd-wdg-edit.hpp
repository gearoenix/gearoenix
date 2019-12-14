#ifndef GEAROENIX_RENDER_WIDGET_EDIT_HPP
#define GEAROENIX_RENDER_WIDGET_EDIT_HPP

#include "../../core/event/cr-ev-listner.hpp"
#include "rnd-wdg-alignment.hpp"
#include "rnd-wdg-widget.hpp"
#include <vector>

namespace gearoenix::physics::animation {
struct Animation;
}

namespace gearoenix::render::font {
class Font2D;
}

namespace gearoenix::render::model {
class Dynamic;
}

namespace gearoenix::render::widget {

struct EditTheme {
    Alignment h_align = Alignment::Center;
    math::Vec4 text_color = math::Vec4(1.0f);
    core::Real text_size = 0.9f;
    math::Vec4 hint_text_color = math::Vec4(0.3f);
    core::Real hint_text_size = 0.9f;
    math::Vec4 background_color = math::Vec4(0.1f, 0.1f, 0.1f, 1.0f);
    math::Vec4 cursor_color = math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    std::uint8_t cursor_width = 4;
    /// Size compare to the height of Edit
    core::Real cursor_size = 0.9f;
    core::Real cursor_state_time = 0.125f;
    core::Real cursor_blink_time = 0.125f;
};
class Edit : public Widget, public core::event::Listener {
public:
    GX_GET_CREF_PRV(std::shared_ptr<font::Font2D>, text_font)
    GX_GET_CREF_PRV(std::wstring, text)
    GX_GET_CREF_PRV(std::vector<core::Real>, text_widths)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, text_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, text_material)
    GX_GET_CREF_PRV(std::wstring, hint_text)
    GX_GET_CREF_PRV(std::vector<core::Real>, hint_text_widths)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, hint_text_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, hint_text_material)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, background_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, background_material)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, cursor_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, cursor_material)
    GX_GET_CREF_PRV(std::shared_ptr<physics::animation::Animation>, cursor_animation)
    GX_GET_CREF_PRV(EditTheme, theme)
    GX_GET_VAL_PRV(bool, actived, true)
    GX_GET_VAL_PRV(bool, left_to_right, true)
    GX_GET_CREF_PRV(std::vector<wchar_t>, left_text)
    GX_GET_CREF_PRV(std::vector<wchar_t>, right_text)
    GX_GET_CREF_PRV(math::Vec2, aspects)
    GX_GET_VAL_PRV(core::Real, cursor_pos_in_text, 0.0f)
    GX_GET_VAL_PRV(core::Real, starting_pos_in_text, 0.0f)
    GX_GET_VAL_PRV(core::Real, starting_text_cut, 0.0f)
    GX_GET_VAL_PRV(core::Real, ending_text_cut, 0.0f)
    GX_GET_VAL_PRV(std::size_t, cursor_index, 0)
    GX_GET_VAL_PRV(std::size_t, temporary_left, 0)
    GX_GET_VAL_PRV(std::size_t, temporary_right, 0)
private:
    void init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    void on_scale() noexcept;
    [[nodiscard]] core::Real compute_starting() noexcept;
    void refill_text() noexcept;

public:
    Edit(
        core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Edit(
        core::Id my_id,
        engine::Engine* e,
        const EditTheme& theme,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Edit(
        core::Id my_id,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Edit() noexcept final;
    void set_text(
        const std::wstring& t,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_hint_text(
        const std::wstring& t,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void active(bool b = true) noexcept;
    [[nodiscard]] bool on_event(const core::event::Data& d) noexcept final;
    void set_left_to_right(bool b) noexcept;
    void insert(wchar_t character, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void remove(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
};
}
#endif
