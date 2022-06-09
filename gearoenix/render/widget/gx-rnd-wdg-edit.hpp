#ifndef GEAROENIX_RENDER_WIDGET_EDIT_HPP
#define GEAROENIX_RENDER_WIDGET_EDIT_HPP

#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-wdg-alignment.hpp"
#include "gx-rnd-wdg-widget.hpp"
#include <vector>

namespace gearoenix::core::event {
struct Engine;
}

namespace gearoenix::physics::animation {
struct Animation;
}

namespace gearoenix::render::font {
struct Font2D;
}

namespace gearoenix::render::material {
struct Unlit;
}

namespace gearoenix::render::model {
struct Dynamic;
}

namespace gearoenix::render::widget {

struct EditTheme {
    Alignment h_align = Alignment::Center;
    math::Vec4<double> text_color = math::Vec4(0.8, 0.8, 0.8, 1.0);
    double text_size = 0.9;
    math::Vec4<double> hint_text_color = math::Vec4(0.7);
    double hint_text_size = 0.9;
    math::Vec4<double> background_color = math::Vec4(0.4, 0.4, 0.4, 1.0);
    math::Vec4<double> cursor_color = math::Vec4(0.0, 0.0, 0.0, 1.0);
    std::uint8_t cursor_width = 4;
    /// Size compare to the height of Edit
    double cursor_size = 0.9;
    double cursor_state_time = 0.125;
    double cursor_blink_time = 0.125;
};
struct Edit final : public Widget, public core::event::Listener {
    GX_GET_CPTR_PRV(core::event::Engine, event_engine)
    GX_GET_CREF_PRV(std::shared_ptr<font::Font2D>, text_font)
    GX_GET_CREF_PRV(std::wstring, text)
    GX_GET_CREF_PRV(std::vector<double>, text_widths)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, text_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Unlit>, text_material)
    GX_GET_CREF_PRV(std::wstring, hint_text)
    GX_GET_CREF_PRV(std::vector<double>, hint_text_widths)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, hint_text_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Unlit>, hint_text_material)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, background_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Unlit>, background_material)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, cursor_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Unlit>, cursor_material)
    GX_GET_CREF_PRV(std::shared_ptr<physics::animation::Animation>, cursor_animation)
    GX_GET_CREF_PRV(EditTheme, theme)
    GX_GET_VAL_PRV(bool, actived, true)
    GX_GET_VAL_PRV(bool, left_to_right, true)
    GX_GET_CREF_PRV(std::vector<wchar_t>, left_text)
    GX_GET_CREF_PRV(std::vector<wchar_t>, right_text)
    GX_GET_CREF_PRV(math::Vec2<double>, aspects)
    GX_GET_VAL_PRV(double, cursor_pos_in_text, 0.0f)
    GX_GET_VAL_PRV(double, starting_text_cut, 0.0f)
    GX_GET_VAL_PRV(double, ending_text_cut, 0.0f)
    GX_GET_VAL_PRV(std::size_t, temporary_left, 0)
    GX_GET_VAL_PRV(std::size_t, temporary_right, 0)
    GX_GET_CREF_PRV(std::weak_ptr<Edit>, edit_self)
private:
    GX_CREATE_GUARD(text)

    void init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& end_call) noexcept;
    void on_scale() noexcept;
    void compute_starting() noexcept;
    void compute_cuts() noexcept;
    void refill_text() noexcept;
    void refill_text_widths() noexcept;
    void place_cursor() noexcept;
    void render_text(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void remove(bool from_left, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

    Edit(
        core::Id id,
        std::string name,
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Edit(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const EditTheme& theme,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Edit(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Edit> construct(
        core::Id id,
        std::string name,
        platform::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Edit> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const EditTheme& theme,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Edit> construct(
        core::Id id,
        std::string name,
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
    void backspace(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void del(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void selected(const math::Vec3<double>& point) noexcept final;
    void clear() noexcept;
};
}
#endif
