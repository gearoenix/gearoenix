#include "gx-rnd-cmr-exposure.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

#include <ImGui/imgui.h>

void gearoenix::render::camera::Exposure::update()
{
    // const auto ev100 = std::log2((aperture * aperture) / shutter_speed * 100.0f / sensitivity);
    // value = 1.0f / (std::pow(2.0f, ev100) * 1.2f);
    value = (aperture * aperture * sensitivity) / (shutter_speed * 120.0f);
}

gearoenix::render::camera::Exposure::Exposure() { update(); }

void gearoenix::render::camera::Exposure::set_aperture(const float a)
{
    GX_ASSERT_D(enabled);
    aperture = a;
    update();
}

float gearoenix::render::camera::Exposure::get_aperture() const
{
    GX_ASSERT_D(enabled);
    return aperture;
}

void gearoenix::render::camera::Exposure::set_shutter_speed(const float s)
{
    GX_ASSERT_D(enabled);
    shutter_speed = s;
    update();
}

float gearoenix::render::camera::Exposure::get_shutter_speed() const
{
    GX_ASSERT_D(enabled);
    return shutter_speed;
}

void gearoenix::render::camera::Exposure::set_sensitivity(const float s)
{
    GX_ASSERT_D(enabled);
    sensitivity = s;
    update();
}

float gearoenix::render::camera::Exposure::get_sensitivity() const
{
    GX_ASSERT_D(enabled);
    return sensitivity;
}

float gearoenix::render::camera::Exposure::get_value() const
{
    GX_ASSERT_D(enabled);
    return value;
}

void gearoenix::render::camera::Exposure::enable()
{
    enabled = true;
    update();
}

void gearoenix::render::camera::Exposure::disable() { enabled = false; }

void gearoenix::render::camera::Exposure::show_debug_gui()
{
    if (enabled) {
        if (ImGui::TreeNode("Exposure")) {
            if (ImGui::InputFloat("Aperture", &aperture, 0.01f, 1.0f, "%.3f")) {
                set_aperture(aperture);
            }
            if (ImGui::InputFloat("Shutter Speed", &shutter_speed, 0.01f, 1.0f, "%.3f")) {
                set_shutter_speed(shutter_speed);
            }
            if (ImGui::InputFloat("Sensitivity", &sensitivity, 0.01f, 1.0f, "%.3f")) {
                set_sensitivity(sensitivity);
            }
            ImGui::Text("Value: %f", value);
            ImGui::TreePop();
        }
    }
}

void gearoenix::render::camera::Exposure::write(platform::stream::Stream& s) const
{
    s.write_fail_debug(enabled);
    if (!enabled) {
        return;
    }
    s.write_fail_debug(aperture);
    s.write_fail_debug(shutter_speed);
    s.write_fail_debug(sensitivity);
}

void gearoenix::render::camera::Exposure::read(platform::stream::Stream& s)
{
    s.read(enabled);
    if (!enabled) {
        return;
    }
    s.read(aperture);
    s.read(shutter_speed);
    s.read(sensitivity);
    update();
}
