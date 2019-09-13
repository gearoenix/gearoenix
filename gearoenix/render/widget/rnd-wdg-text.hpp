#ifndef GEAROENIX_RENDER_WIDGET_TEXT_HPP
#define GEAROENIX_RENDER_WIDGET_TEXT_HPP
#include "../../core/cr-static.hpp"
#include "rnd-wdg-widget.hpp"
#include "rnd-wdg-alignment.hpp"

namespace gearoenix::render::font {
class Font2D;
}

namespace gearoenix::render::widget {
class Text : public Widget {
    GX_GET_CREF_PRV(std::wstring, text)
    GX_GET_CREF_PRV(std::shared_ptr<font::Font2D>, text_font)
    GX_GET_CREF_PRV(math::Vec3, text_color)
    GX_GET_VAL_PRV(Alignment, align, Alignment::CenterMiddle)
    GX_GET_VAL_PRV(core::Id, text_mesh_id, 0)
    GX_GET_VAL_PRV(core::Real, current_x_scale, 1.0f)
public:
    Text(
        const core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Text(
        const core::Id my_id,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Text() noexcept final;
	void set_text(const std::wstring& t, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})) noexcept;
	void set_text_color(core::Real red, core::Real green, core::Real blue, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c = core::sync::EndCaller<core::sync::EndCallerIgnore>([] {})) noexcept;
};
}
#endif
