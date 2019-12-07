#ifndef GEAROENIX_RENDER_WIDGET_EDIT_HPP
#define GEAROENIX_RENDER_WIDGET_EDIT_HPP
#include "rnd-wdg-alignment.hpp"
#include "rnd-wdg-widget.hpp"

namespace gearoenix::render::font {
class Font2D;
}

namespace gearoenix::render::model {
class Dynamic;
}

namespace gearoenix::render::widget {
class Edit : public Widget {
    GX_GET_CREF_PRV(std::shared_ptr<font::Font2D>, text_font)
    GX_GET_CREF_PRV(std::wstring, text)
    GX_GET_CREF_PRV(std::vector<core::Real>, text_widths)
    GX_GET_CREF_PRV(math::Vec4, text_color)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, text_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, text_material)
    GX_GET_CREF_PRV(std::wstring, hint_text)
    GX_GET_CREF_PRV(std::vector<core::Real>, hint_text_widths)
    GX_GET_CREF_PRV(math::Vec4, hint_text_color)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, hint_text_model)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, hint_text_material)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, background)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, background_material)
    GX_GET_CREF_PRV(std::shared_ptr<model::Dynamic>, cursor)
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, cursor_material)
    GX_GET_VAL_PRV(Alignment, v_align, Alignment::Center)
    GX_GET_VAL_PRV(Alignment, h_align, Alignment::Center)
public:
    Edit(
        core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
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
