#ifndef GEAROENIX_CORE_UI_UI_HPP
#define GEAROENIX_CORE_UI_UI_HPP

#include "cr-ev-event.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace ui {
            class Ui : public Event {
            public:
                typedef enum : core::Id {
                    PRESSED = 1,
                    CANCELED = 2,
                    CLICKED = 3,
                } ActionType;

            protected:
                const ActionType action_type;
                const core::Id widget_id;

            public:
                Ui(ActionType action_type, core::Id widget_id);
                virtual ~Ui();
                ActionType get_action() const;
                core::Id get_widget() const;
            };
        }
    }
}
}
#endif
