#ifndef GEAROENIX_SYSTEM_CONFIGURATION_HPP
#define GEAROENIX_SYSTEM_CONFIGURATION_HPP
#include "../core/cr-build-configuration.hpp"
#include "../render/engine/rnd-eng-configuration.hpp"
#include <string>
namespace gearoenix::system {
	struct Configuration {
		render::engine::Configuration render_config;
	};
}
#endif