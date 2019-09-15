#ifndef GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#define GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#include "../cr-types.hpp"
#include "../../math/math-vector.hpp"
#include <chrono>
namespace gearoenix::core::event::movement {
struct Base {
	std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();
	
	math::Vec3 current_position;
	math::Vec3 previous_position;

	math::Vec3 delta_position;
	Real delta_time;

	math::Vec3 speed;

	void update(const math::Vec3& p) noexcept {
		previous_position = current_position;
		previous_time = current_time;
		current_position = p;
		delta_position = current_position - previous_position;
		current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<Real> dt = current_time - previous_time;
		delta_time = dt.count();
		speed = delta_position / delta_time;
	}
};
}
#endif
