#ifndef GEAROENIX_RENDER_WIDGET_MODAL_HPP
#define GEAROENIX_RENDER_WIDGET_MODAL_HPP
#include "rnd-wdg-widget.hpp"
#include <functional>

namespace gearoenix::render::widget {
class Button;
class Modal final : public Widget {
    GX_GET_CREF_PRV(std::weak_ptr<Modal>, modal_self)
private:
    std::shared_ptr<Button> close_mdl;
    std::function<void()> on_close = []() noexcept {};

    Modal(
        core::Id id,
        std::string name,
        system::stream::Stream* s,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Modal(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Modal> construct(
        core::Id id,
        std::string name,
        system::stream::Stream* s,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Modal> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Modal() noexcept final;
    void set_scene(scene::Scene* s) noexcept final;
    void set_on_close(const std::function<void()>& f) noexcept;
};
}
#endif
