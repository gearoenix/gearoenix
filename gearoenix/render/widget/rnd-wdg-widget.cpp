#include "rnd-wdg-widget.hpp"
//#include "../../core/cr-application.hpp"
//#include "../../core/event/cr-ev-ui-ui.hpp"
//#include "../../system/stream/sys-stm-stream.hpp"
//#include "../../system/sys-app.hpp"
//#include "../engine/rnd-eng-engine.hpp"
//#include "rnd-wdg-button.hpp"
//#include "rnd-wdg-text.hpp"
//

//
//void gearoenix::render::widget::Widget::press_effect()
//{
//}
//
//void gearoenix::render::widget::Widget::release_effect()
//{
//}
//
//void gearoenix::render::widget::Widget::cancel_effect()
//{
//}
//
//gearoenix::render::widget::Widget* gearoenix::render::widget::Widget::read(
//	const core::Id my_id,
//	const std::shared_ptr<system::stream::Stream>& f,
//	const std::shared_ptr<engine::Engine>& e,
//	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
//{
//    core::Id t;
//    f->read(t);
//    switch (t) {
//    case BUTTON:
//        return new Button(my_id, f, e, c);
//    case TEXT:
//        return new Text(my_id, f, e, c);
//    default:
//        GXUNEXPECTED;
//    }
//    return nullptr;
//}
//
//gearoenix::render::widget::Widget::~Widget()
//{
//}
//
//void gearoenix::render::widget::Widget::state_change(EventType e)
//{
//    //std::lock_guard<std::mutex> lg(event_locker);
//    core::Application* coreapp = render_engine->get_system_application()->get_core_app();
//    switch (state_type) {
//    case StateType::NORMAL:
//        switch (e) {
//        case EventType::PRESS:
//            state_type = StateType::PRESSED;
//            press_effect();
//            coreapp->on_event(core::event::ui::Ui(core::event::ui::Ui::ActionType::PRESSED, asset_id));
//            break;
//        default:
//            break;
//        }
//        break;
//    case StateType::PRESSED:
//        switch (e) {
//        case EventType::RELEASE:
//            state_type = StateType::NORMAL;
//            release_effect();
//            coreapp->on_event(core::event::ui::Ui(core::event::ui::Ui::ActionType::CLICKED, asset_id));
//            break;
//        case EventType::MOVE_OUT:
//            state_type = StateType::NORMAL;
//            cancel_effect();
//            coreapp->on_event(core::event::ui::Ui(core::event::ui::Ui::ActionType::CANCELED, asset_id));
//            break;
//        default:
//            break;
//        }
//        break;
//    default:
//        break;
//    }
//    //(void)lg;
//}

void gearoenix::render::widget::Widget::press_effect() noexcept
{
}

void gearoenix::render::widget::Widget::release_effect() noexcept
{
}

void gearoenix::render::widget::Widget::cancel_effect() noexcept
{
}

gearoenix::render::widget::Widget::Widget(const core::Id my_id, system::stream::Stream*const s, engine::Engine*const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : model::Model(my_id, s, e, c)
{
}

//gearoenix::render::widget::Widget::Widget(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
//    : model::Model(e, c)
//{
//}

gearoenix::render::widget::Widget::~Widget() noexcept
{
}

void gearoenix::render::widget::Widget::read_gx3d(const std::shared_ptr<system::stream::Stream>& f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GXUNIMPLEMENTED;
}

void gearoenix::render::widget::Widget::state_change(EventType e) noexcept
{
}
