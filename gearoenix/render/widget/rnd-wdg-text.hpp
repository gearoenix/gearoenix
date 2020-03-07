#ifndef GEAROENIX_RENDER_WIDGET_TEXT_HPP
#define GEAROENIX_RENDER_WIDGET_TEXT_HPP
#include "../../core/cr-static.hpp"
#include "../../math/math-vector-4d.hpp"
#include "rnd-wdg-alignment.hpp"
#include "rnd-wdg-widget.hpp"

namespace gearoenix::render::font {
class Font2D;
}

namespace gearoenix::render::widget {
class Text : public Widget {
    GX_GET_CREF_PRT(std::wstring, text)
    GX_GET_CREF_PRT(std::shared_ptr<font::Font2D>, text_font)
    GX_GET_CREF_PRT(math::Vec4<double>, text_color)
    GX_GET_VAL_PRT(Alignment, v_align, Alignment::Center)
    GX_GET_VAL_PRT(Alignment, h_align, Alignment::Center)
    GX_GET_VAL_PRT(core::Id, text_mesh_id, 0)
public:
    Text(
        core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Text(
        core::Id my_id,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Text() noexcept override;
    void set_text(
        const std::wstring& t,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_text_color(
        double red, double green, double blue,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void set_text_color(
        const math::Vec4<double>& v,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
};
}
#endif
