#include "gx-rnd-cmr-exposure.hpp"
#include <cmath>

void gearoenix::render::camera::ExposureData::update() noexcept
{
    //    const auto ev100 = std::log2((aperture * aperture) / shutter_speed * 100.0f / sensitivity);
    //    value = 1.0f / (std::pow(2.0f, ev100) * 1.2f);
    value = (shutter_time * sensitivity) / (aperture * aperture * 120.0f);
}

gearoenix::render::camera::ExposureData::ExposureData() noexcept
{
    update();
}

void gearoenix::render::camera::ExposureData::set_aperture(const float a) noexcept
{
    aperture = a;
    update();
}

void gearoenix::render::camera::ExposureData::set_shutter_time(const float s) noexcept
{
    shutter_time = s;
    update();
}

void gearoenix::render::camera::ExposureData::set_sensitivity(const float s) noexcept
{
    sensitivity = s;
    update();
}
