#ifndef GEAROENIX_RENDER_CAMERA_EXPOSURE_HPP
#define GEAROENIX_RENDER_CAMERA_EXPOSURE_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"

namespace gearoenix::render::camera {
struct ExposureData final {
    GX_GET_VAL_PRV(float, aperture, 4.0f);
    GX_GET_VAL_PRV(float, shutter_time, aperture* aperture * 120.0f);
    GX_GET_VAL_PRV(float, sensitivity, 1.0f);
    GX_GET_VAL_PRV(float, value, 1.0f);

    void update() noexcept;

public:
    ExposureData() noexcept;
    void set_aperture(float) noexcept;
    void set_shutter_time(float) noexcept;
    void set_sensitivity(float) noexcept;
};
}

#endif