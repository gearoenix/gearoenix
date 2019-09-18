#ifndef GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#define GEAROENIX_RENDER_WIDGET_BUTTON_HPP
#include "rnd-wdg-widget.hpp"
#include <chrono>
namespace gearoenix::physics::animation {
    struct Animation;
}
namespace gearoenix::render::widget {
    class Button : public Widget {
        GX_GETSET_AVAL_PRT(core::Real, current_scale, 1.0f)
    protected:
        std::weak_ptr<physics::animation::Animation> animation;
        std::function<void()> on_click = []{};

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
        ~Button() noexcept;
        void selected(const math::Vec3& point) noexcept final;
        void select_cancelled() noexcept final;
        void select_released(const math::Vec3& point) noexcept final;
        void set_on_click(const std::function<void()>& on_click) noexcept;
    };
}
#endif
