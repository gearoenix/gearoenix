#ifndef GEAROENIX_CORE_EVENT_WINDOW_RESIZE_HPP
#define GEAROENIX_CORE_EVENT_WINDOW_RESIZE_HPP

#include "cr-ev-event.hpp"

namespace gearoenix {
namespace core {
    namespace event {
        class WindowResize : public Event {
        private:
            Real pre_width, pre_height, cur_width, cur_height;

        public:
            WindowResize(Real pre_width, Real pre_height, Real cur_width, Real cur_height);
            Real get_previous_width() const;
            Real get_previous_height() const;
            Real get_current_width() const;
            Real get_current_height() const;
        };
    }
}
}
#endif
