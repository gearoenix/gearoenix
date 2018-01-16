#ifndef GEAROEMIX_CORE_EVENT_HPP
#define GEAROEMIX_CORE_EVENT_HPP

#include "cr-types.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        class WindowResize;
        class Event {
        public:
            enum Type {
                WINDOW_RESIZE,
            };
            virtual ~Event();
            virtual Type get_type() const = 0;
            const WindowResize* to_window_resize() const;
        };

        class WindowResize : public Event {
        private:
            Real pre_width, pre_height, cur_width, cur_height;

        public:
            WindowResize(Real pre_width, Real pre_height, Real cur_width, Real cur_height);
            Type get_type() const;
        };
    }
}
}
#endif
