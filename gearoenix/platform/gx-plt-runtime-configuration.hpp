#pragma once
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::platform {
struct RuntimeConfiguration final {
    GX_GETSET_CREF_PRV(std::string, application_name);
    GX_GETSET_VAL_PRV(bool, fullscreen, false);
    GX_GETSET_VAL_PRV(bool, landscape, true);
    GX_GETSET_VAL_PRV(std::uint16_t, window_width, 1280);
    GX_GETSET_VAL_PRV(std::uint16_t, window_height, 720);
    GX_GETSET_VAL_PRV(std::uint16_t, window_resizing_event_interval_ms, 500);
    GX_GETSET_VAL_PRV(bool, show_cursor, true);

    // Following settings are in platform because actually before engine creation we don't have any render settings
    GX_GETSET_VAL_PRV(bool, vulkan_render_backend_enabled, true);
    GX_GETSET_VAL_PRV(bool, direct3dx_render_backend_enabled, true);
    GX_GETSET_VAL_PRV(bool, metal_render_backend_enabled, true);
    GX_GETSET_VAL_PRV(bool, opengl_render_backend_enabled, true);

    void write(stream::Stream&) const;
    void read(stream::Stream&);

    RuntimeConfiguration();

public:
    RuntimeConfiguration(const RuntimeConfiguration&) = delete;
    RuntimeConfiguration(RuntimeConfiguration&&) = delete;
    [[nodiscard]] static RuntimeConfiguration& get();
    ~RuntimeConfiguration();
    void show_debug_gui();
};
}