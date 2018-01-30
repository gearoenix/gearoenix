#ifndef GEAROEMIX_CORE_EVENT_EVENT_HPP
#define GEAROEMIX_CORE_EVENT_EVENT_HPP

#include "../cr-types.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        namespace button {
            class Button;
        }
        namespace movement {
            class Movement;
        }
        class WindowResize;
        class Event {
        public:
            typedef enum : core::Id {
                WINDOW_RESIZE,
                BUTTON,
                MOVEMENT,
                USER_DEFINED,
            } EventType;

        private:
            const EventType event_type;

        protected:
            Event(EventType t);

        public:
            virtual ~Event();
            EventType get_type() const;
            const button::Button& to_button() const;
            const movement::Movement& to_movement() const;
            const WindowResize& to_window_resize() const;
        };
    }
}
}
#endif
