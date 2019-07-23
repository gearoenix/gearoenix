#ifndef GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#include "../model/rnd-mdl-model.hpp"
#include "rnd-wdg-type.hpp"

namespace gearoenix::render::widget {
class Widget : public model::Model {
public:
    typedef enum : core::Id {
        NORMAL,
        PRESSED,
    } StateType;

    // This is something internal, it's gonna be used between ui-scene & widget
    typedef enum : core::Id {
        PRESS,
        RELEASE,
        MOVE_OUT,
    } EventType;

protected:
    StateType state_type = StateType::NORMAL;
    // at the end it must send event
    virtual void press_effect() noexcept;
    virtual void release_effect() noexcept;
    virtual void cancel_effect() noexcept;

	Widget(
		core::Id my_id,
		system::stream::Stream* s,
		engine::Engine* e,
		const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
	Widget(
		engine::Engine* e,
		const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
public:
    virtual ~Widget() noexcept;
    static std::shared_ptr<Widget> read_gx3d(
		core::Id my_id,
		system::stream::Stream* f,
		engine::Engine* e,
		const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    void state_change(EventType e) noexcept;
};
}
#endif
