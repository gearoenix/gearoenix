#ifndef GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#define GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#include "rnd-wdg-widget.hpp"
#include <chrono>
namespace gearoenix::physics::animation {
    struct Animation;
}
namespace gearoenix::render::widget {
    class Button : public Widget {
        GX_GETSET_AVAL_PRT(core::Real, scale, 1.0f)
    protected:
        std::shared_ptr<physics::animation::Animation> animation;

    public:
        Button(
            core::Id my_id,
            system::stream::Stream* f,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        Button(
            core::Id my_id,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        virtual ~Button() noexcept;
        virtual void pressed() noexcept;
        virtual void cancelled() noexcept;
        virtual void released() noexcept;
    };
}
#endif
