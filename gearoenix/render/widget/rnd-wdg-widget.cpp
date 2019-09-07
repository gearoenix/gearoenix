#include "rnd-wdg-widget.hpp"
//#include "../../core/cr-application.hpp"
//#include "../../core/event/cr-ev-ui-ui.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
//#include "../../system/sys-app.hpp"
//#include "../engine/rnd-eng-engine.hpp"
//#include "rnd-wdg-button.hpp"
#include "rnd-wdg-text.hpp"

gearoenix::render::widget::Widget::Widget(const core::Id my_id, const Type t, system::stream::Stream* const s, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : model::Model(my_id, s, e, c)
    , widget_type(t)
{
}

gearoenix::render::widget::Widget::Widget(const core::Id my_id, const Type t, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : model::Model(my_id, e, c)
    , widget_type(t)
{
}

gearoenix::render::widget::Widget::~Widget() noexcept
{
}

std::shared_ptr<gearoenix::render::widget::Widget> gearoenix::render::widget::Widget::read_gx3d(
	core::Id my_id, 
	system::stream::Stream* f, 
	engine::Engine* e, 
	const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const auto t = f->read<Type>();
	switch (t)
	{
	case Type::Text:
		return std::make_shared<Text>(my_id, f, e, c);
	}
	GXLOGF("Unexpected widget type (" << static_cast<core::TypeId>(t) << ")  in: " << my_id);
}