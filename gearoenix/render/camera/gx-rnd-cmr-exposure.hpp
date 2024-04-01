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

    void update();

public:
    Exposure();
    void set_aperture(float);
    [[nodiscard]] float get_aperture() const;
    void set_shutter_speed(float);
    [[nodiscard]] float get_shutter_speed() const;
    void set_sensitivity(float);
    [[nodiscard]] float get_sensitivity() const;
    [[nodiscard]] float get_value() const;
    void enable();
    void disable();
    void show_debug_gui();
};
}

#endif