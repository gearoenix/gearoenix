#ifndef GEAROENIX_RENDER_CAMERA_EXPOSURE_HPP
#define GEAROENIX_RENDER_CAMERA_EXPOSURE_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"

namespace gearoenix::render::camera {
struct Exposure final {
    GX_GET_VAL_PRV(bool, enabled, true);

private:
    float aperture = 4.0f;
    float shutter_speed = aperture * aperture / 120.0f;
    float sensitivity = 1.0f;
    float value = 1.0f;

    void update() noexcept;

public:
    Exposure() noexcept;
    void set_aperture(float) noexcept;
    [[nodiscard]] float get_aperture() const noexcept;
    void set_shutter_speed(float) noexcept;
    [[nodiscard]] float get_shutter_speed() const noexcept;
    void set_sensitivity(float) noexcept;
    [[nodiscard]] float get_sensitivity() const noexcept;
    [[nodiscard]] float get_value() const noexcept;
    void enable() noexcept;
    void disable() noexcept;
    void show_debug_gui() noexcept;
};
}

#endif