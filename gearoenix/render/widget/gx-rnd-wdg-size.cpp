#include "gx-rnd-wdg-size.hpp"
#include "../../platform/gx-plt-application.hpp"

double gearoenix::render::widget::Size::to_world() const
{
    switch (type_index) {
    case Type::WidthBased: {
        const auto& wsz = platform::Application::get().get_base().get_window_size();
        return value * static_cast<double>(wsz.x) / static_cast<double>(wsz.y);
    }
    case Type::HeightBased:
        return value;
    case Type::PixelBased: {
        return value / static_cast<double>(platform::Application::get().get_base().get_window_size().y);
    }
    }
    return 0;
}
