#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "rnd-wdg-widget.hpp"
#include <functional>

namespace gearoenix::render::widget {
class Modal : public Widget {
private:
    model::Model* close_mdl = nullptr;
    std::function<void()> on_close = [] () noexcept {};

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
    void set_scene(scene::Scene* s) noexcept;
    void selected_on(const math::Vec3& point, const std::vector<model::Model*>& children) noexcept final;
    void set_on_close(const std::function<void()>& f) noexcept;
};
}
#endif
