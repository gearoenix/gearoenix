#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "rnd-wdg-widget.hpp"

namespace gearoenix::render::widget {
class Modal : public Widget {
private:
    model::Model* close_mdl = nullptr;

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
    void selected(const math::Vec3& point, const std::vector<model::Model*>& children) noexcept final;
};
}
#endif
