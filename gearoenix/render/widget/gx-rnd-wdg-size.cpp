#include "gx-rnd-wdg-size.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"

double gearoenix::render::widget::Size::to_world(const engine::Engine& e) const
{
    switch (type_index) {
    case Type::WidthBased: {
        const auto& wsz = e.get_platform_application().get_base().get_window_size();
        return value * static_cast<double>(wsz.x) / static_cast<double>(wsz.y);
    }
    case Type::HeightBased:
        return value;
    case Type::PixelBased: {
        return value / static_cast<double>(e.get_platform_application().get_base().get_window_size().y);
    }
    }
    return 0;
}
