#ifndef GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#define GEAROENIX_RENDER_WIDGET_WIDGET_HPP
#include "../model/rnd-mdl-model.hpp"
#include "rnd-wdg-type.hpp"

namespace gearoenix::render::widget {
class Widget : public model::Model {
    GX_GET_CVAL_PRT(Type, widget_type)
protected:
	Widget(
		core::Id my_id,
        Type t,
		system::stream::Stream* s,
		engine::Engine* e,
		const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
	Widget(
        core::Id my_id,
        Type t,
		engine::Engine* e,
		const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
public:
    virtual ~Widget() noexcept;
    static std::shared_ptr<Widget> read_gx3d(
		core::Id my_id,
		system::stream::Stream* f,
		engine::Engine* e,
		const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
};
}
#endif
