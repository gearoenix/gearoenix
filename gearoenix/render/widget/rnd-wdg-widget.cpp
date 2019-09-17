#include "rnd-wdg-widget.hpp"
//#include "../../core/cr-application.hpp"
//#include "../../core/event/cr-ev-ui-ui.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
//#include "../../system/sys-app.hpp"
#include "../model/rnd-mdl-transformation.hpp"
//#include "rnd-wdg-button.hpp"
#include "rnd-wdg-text.hpp"

gearoenix::render::widget::Widget::Widget(
    const core::Id my_id,
    const Type t,
    system::stream::Stream* const s,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : model::Model(my_id, model::Type::Widget, new model::Transformation(&uniform, &occlusion_sphere, this), s, e, c)
    , widget_type(t)
{
}

gearoenix::render::widget::Widget::Widget(
    const core::Id my_id,
    const Type t,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : model::Model(my_id, model::Type::Widget, new model::Transformation(&uniform, &occlusion_sphere, this), e, c)
    , widget_type(t)
{
}

gearoenix::render::widget::Widget::~Widget() noexcept
{
}

std::shared_ptr<gearoenix::render::widget::Widget> gearoenix::render::widget::Widget::read_gx3d(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const auto t = f->read<Type>();
    switch (t) {
    case Type::Text:
        return std::make_shared<Text>(my_id, f, e, c);
    default:
        GXLOGF("Unexpected widget type (" << static_cast<core::TypeId>(t) << ")  in: " << my_id);
    }
}

bool gearoenix::render::widget::Widget::get_dynamicity() const noexcept
{
    return true;
}

void gearoenix::render::widget::Widget::selected(const math::Vec3&) noexcept
{
}

void gearoenix::render::widget::Widget::selected_on(const math::Vec3&, const std::vector<model::Model*>&) noexcept
{
}

void gearoenix::render::widget::Widget::select_cancelled(const math::Vec3&) noexcept
{
}

void gearoenix::render::widget::Widget::select_released(const math::Vec3&) noexcept
{
}

void gearoenix::render::widget::Widget::dragged(const math::Vec3&) noexcept
{
}

void gearoenix::render::widget::Widget::dragged_on(const math::Vec3&, const std::vector<model::Model*>&) noexcept
{
}
