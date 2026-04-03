#include "gx-rnd-wdg-size.hpp"
#include "../../platform/gx-plt-application.hpp"

gearoenix::core::fp_t gearoenix::render::widget::Size::to_world() const
{
    switch (type_index) {
    case Type::WidthBased: {
        const auto& wsz = platform::Application::get().get_base().get_window_size();
        return value * static_cast<core::fp_t>(wsz.x) / static_cast<core::fp_t>(wsz.y);
    }
    case Type::HeightBased:
        return value;
    case Type::PixelBased: {
        return value / static_cast<core::fp_t>(platform::Application::get().get_base().get_window_size().y);
    }
    }
    return 0;
}
