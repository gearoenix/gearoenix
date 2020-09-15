#ifndef GEAROENIX_SYSTEM_CONFIGURATION_HPP
#define GEAROENIX_SYSTEM_CONFIGURATION_HPP
#include "../render/engine/gx-rnd-eng-configuration.hpp"

namespace gearoenix::system {
struct Configuration {
    render::engine::Configuration render_config;
};
}
#endif