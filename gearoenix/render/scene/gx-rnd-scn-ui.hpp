#ifndef GEAROENIX_RENDER_SCENE_UI_HPP
#define GEAROENIX_RENDER_SCENE_UI_HPP
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "gx-rnd-scn-scene.hpp"
#include <chrono>

namespace gearoenix::render::widget {
struct Widget;
struct Edit;
}

namespace gearoenix::render::scene {
struct Ui final : public Scene, public core::event::Listener {
private:
    void init() noexcept;
    GX_CREATE_GUARD(events)
    std::map<core::event::touch::FingerId, std::weak_ptr<widget::Widget>> selected_widgets;
    std::weak_ptr<widget::Edit> active_edit;

    void find_hit_widgets(double x, double y,
        const std::function<void(widget::Widget* const, const math::Vec3<double>&)>& f1,
        const std::function<void(widget::Widget* const, const math::Vec3<double>&, const std::vector<model::Model*>&)>& f2,
        const std::function<void()>& f3) const noexcept;

    void pointer_down(double x, double y, core::event::touch::FingerId finger_id) noexcept;
    void pointer_up(core::event::touch::FingerId finger_id) noexcept;
    void pointer_move(double x, double y, core::event::touch::FingerId finger_id) noexcept;

public:
    Ui(
        core::Id my_id, std::string name, platform::stream::Stream* f, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Ui(
        core::Id my_id, std::string name, engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Ui() noexcept final;
    [[nodiscard]] bool on_event(const core::event::Data& d) noexcept final;

    void add_model(const std::shared_ptr<model::Model>& m) noexcept final;
    void set_enabled(bool) noexcept final;
};
}
#endif
