#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "rnd-wdg-widget.hpp"
#include <functional>

namespace gearoenix::render::widget {
class Button;
class Modal : public Widget {
private:
    std::shared_ptr<Button> close_mdl;
    std::function<void()> on_close = []() noexcept {};

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
    ~Modal() noexcept final;
    void set_scene(scene::Scene* s) noexcept final;
    void set_on_close(const std::function<void()>& f) noexcept;
};
}
#endif
