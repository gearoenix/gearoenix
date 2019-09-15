#ifndef GEAROENIX_CORE_EVENT_EVENT_HPP
#define GEAROENIX_CORE_EVENT_EVENT_HPP
#include "cr-ev-button.hpp"
#include "cr-ev-gesture.hpp"
#include "cr-ev-id.hpp"
#include "cr-ev-movement.hpp"
#include "cr-ev-system.hpp"
#include <variant>
namespace gearoenix::core::event {
struct Data {
    Id source = Id::None;
    std::variant<
		button::KeyboardData,
        button::MouseData,
        button::MouseScroll,
		gesture::Drag,
		gesture::MouseDrag,
		movement::Base,
        system::WindowSizeChangeData,
        int>
        data = 0;
};
}
#endif
