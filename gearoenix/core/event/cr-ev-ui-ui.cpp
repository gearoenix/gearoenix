#include "cr-ev-ui-ui.hpp"

gearoenix::core::event::ui::Ui::Ui(ActionType action_type, core::Id widget_id)
    : Event(Event::EventType::UI)
    , action_type(action_type)
    , widget_id(widget_id)
{
}

gearoenix::core::event::ui::Ui::~Ui() {}

gearoenix::core::event::ui::Ui::ActionType gearoenix::core::event::ui::Ui::get_action() const
{
    return action_type;
}
gearoenix::core::Id gearoenix::core::event::ui::Ui::get_widget() const
{
    return widget_id;
}
