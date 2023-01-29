#ifndef GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include "gx-rnd-wdg-type.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::widget {
struct Layout;
struct Widget : public core::event::Listener {
    typedef std::map<std::pair<double /*event-priority*/, std::string>, std::shared_ptr<Widget>> ChildrenMap;

    engine::Engine& e;
    const std::string name;
    const Type widget_type;

    GX_GET_VAL_PRT(bool, sensitivity, true);
    GX_GET_CREF_PRT(ChildrenMap, children);
    GX_GET_CREF_PRT(std::shared_ptr<Layout>, layout);
    GX_GET_CREF_PRT(std::function<void(const math::Vec3<double>&)>, on_press);
    GX_GET_CREF_PRT(std::function<void(const math::Vec3<double>&)>, on_release);
    GX_GET_CREF_PRT(std::function<void(const math::Vec3<double>&)>, on_click);
    GX_GET_CREF_PRT(std::function<void()>, on_cancel);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, model_entity_id, 0);
    GX_GET_VAL_PRT(core::ecs::entity_id_t, camera_entity_id, 0);
    GX_GET_VAL_PRT(bool, is_pressed, false);

protected:
    [[nodiscard]] std::optional<math::Vec3<double>> get_hit_point(const math::Vec2<double>& normalised_point) const noexcept;
    void handle_click_gesture(const core::event::Data& event_data) noexcept;
    void handle_button_mouse(const core::event::Data& event_data) noexcept;
    void handle_movement_mouse(const core::event::Data& event_data) noexcept;
    void handle_mouse_outside() noexcept;

public:
    Widget(std::string&& name, Type t, engine::Engine& e) noexcept;
    ~Widget() noexcept override;
    virtual void set_on_press(const std::function<void(const math::Vec3<double>&)>& fun) noexcept;
    virtual void set_on_release(const std::function<void(const math::Vec3<double>&)>& fun) noexcept;
    virtual void set_on_cancel(const std::function<void()>& fun) noexcept;
    virtual void set_on_click(const std::function<void(const math::Vec3<double>&)>& fun) noexcept;
    virtual void set_sensitivity(bool) noexcept;
    virtual void set_model_entity_id(core::ecs::entity_id_t model_entity_id) noexcept;
    virtual void set_camera_entity_id(core::ecs::entity_id_t camera_entity_id) noexcept;
    virtual void register_for_events() noexcept;
    virtual void set_layout(std::shared_ptr<Layout> layout) noexcept;
    virtual void show() noexcept;
    virtual void hide() noexcept;
    virtual void add_child(std::shared_ptr<Widget>&& child, double priority) noexcept;
    void add_child(std::shared_ptr<Widget>&& child) noexcept;
    core::event::Listener::Response on_event(const core::event::Data& event_data) noexcept override;
};
}
#endif
