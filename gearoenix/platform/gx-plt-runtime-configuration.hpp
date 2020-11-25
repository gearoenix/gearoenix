#ifndef GEAROENIX_SYSTEM_RUNTIME_CONFIGURATION_HPP
#define GEAROENIX_SYSTEM_RUNTIME_CONFIGURATION_HPP

#include <cstdint>

namespace gearoenix::platform {
struct RuntimeConfiguration {
    bool fullscreen = false;
    std::uint16_t window_width = 1000;
    std::uint16_t window_height = 700;
    bool show_cursor = true;
};
}
#endif