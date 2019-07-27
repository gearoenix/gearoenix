#ifndef GEAROENIX_CORE_EVENT_EVENT_HPP
#define GEAROENIX_CORE_EVENT_EVENT_HPP
#include "cr-ev-bt-button.hpp"
#include "cr-ev-sys-system.hpp"
#include "cr-ev-id.hpp"
#include <variant>
namespace gearoenix::core::event {
	struct Data {
		Id source;
		std::variant<
			button::Data,
            system::WindowSizeChangeData,
            int
		> data;
	};
}
#endif
