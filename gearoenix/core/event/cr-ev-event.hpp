#ifndef GEAROENIX_CORE_EVENT_EVENT_HPP
#define GEAROENIX_CORE_EVENT_EVENT_HPP
#include "cr-ev-bt-button.hpp"
#include <variant>
namespace gearoenix::core::event {
	struct Data {
		enum struct Source {
			BUTTON_MOUSE,
		} s;
		std::variant<
			button::Data
		> d;
	};
}
#endif
