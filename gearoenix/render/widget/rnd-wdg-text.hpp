#ifndef GEAROENIX_RENDER_WIDGET_TEXT_HPP
#define GEAROENIX_RENDER_WIDGET_TEXT_HPP
#include "rnd-wdg-widget.hpp"
#include "rnd-wdg-alignment.hpp"

namespace gearoenix::render::font {
class Font2D;
}

namespace gearoenix::render::widget {
class Text : public Widget {
protected:
    std::wstring text;
    Alignment::Id align;;
    std::shared_ptr<font::Font2D> fnt;
	core::Id text_mesh_id;

public:
    Text(
        const core::Id my_id,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Text() noexcept final;
};
}
#endif
