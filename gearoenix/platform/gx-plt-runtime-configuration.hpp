#ifndef GEAROENIX_PLATFORM_RUNTIME_CONFIGURATION_HPP
#define GEAROENIX_PLATFORM_RUNTIME_CONFIGURATION_HPP

#include "../core/gx-cr-build-configuration.hpp"
#include "../render/gx-rnd-runtime-configuration.hpp"
#include <string>

namespace gearoenix::platform {
struct RuntimeConfiguration {
    GX_GETSET_CREF_PRV(std::string, application_name)
    GX_GETSET_VAL_PRV(bool, fullscreen, false)
    GX_GETSET_VAL_PRV(std::uint16_t, window_width, 1000)
    GX_GETSET_VAL_PRV(std::uint16_t, window_height, 700)
    GX_GETSET_VAL_PRV(std::uint16_t, window_resizing_event_interval_ms, 500)
    GX_GETSET_VAL_PRV(bool, show_cursor, true)
    GX_GET_REF_PRV(render::RuntimeConfiguration, render_configuration)

    RuntimeConfiguration() noexcept
        : application_name(GX_APPLICATION_NAME)
    {
    }
};
}
#endif