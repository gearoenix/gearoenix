#ifndef GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#define GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#include "../cr-types.hpp"
#include "../../math/math-vector.hpp"
#include <chrono>
namespace gearoenix::core::event::movement {
struct Data {
	std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point now_time = std::chrono::high_resolution_clock::now();
    math::Vec2 position; 
	math::Vec2 previous_position;
	math::Vec2 delta_position;

	void update() noexcept {
		delta_position = position - previous_position;
		now_time = std::chrono::high_resolution_clock::now();
	}
};
}
#endif
