#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "rnd-wdg-widget.hpp"

namespace gearoenix::render::widget {
    class Modal : public Widget {
    public:
        Modal(
            core::Id my_id,
            system::stream::Stream* s,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        Modal(
            core::Id my_id,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        ~Modal() noexcept;
    };
}
#endif
