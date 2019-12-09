#ifndef GEAROENIX_RENDER_WIDGET_EDIT_HPP
#define GEAROENIX_RENDER_WIDGET_EDIT_HPP
#include "rnd-wdg-alignment.hpp"
#include "rnd-wdg-widget.hpp"

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
class Edit : public Widget {
public:
    struct Theme {
        Alignment v_align = Alignment::Center;
        Alignment h_align = Alignment::Center;
        math::Vec4 text_color = math::Vec4(1.0f);
        math::Vec4 hint_text_color = math::Vec4(0.3f);
        math::Vec4 background_color = math::Vec4(0.1f, 0.1f, 0.1f, 1.0f);
        math::Vec4 cursor_color = math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        std::uint8_t cursor_width = 2;
        /// Size compare to the height of Edit
        core::Real cursor_size = 0.9f;
        core::Real cursor_state_time = 0.125f;
        core::Real cursor_blink_time = 0.125f;
    };
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
    GX_GET_CREF_PRV(Theme, theme)
private:
    void init(const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    void on_scale() noexcept;

public:
    Edit(
        core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Edit(
        core::Id my_id,
        engine::Engine* e,
        const Theme& theme,
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
};
}
#endif
