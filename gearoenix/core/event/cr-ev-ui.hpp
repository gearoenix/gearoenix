#ifndef GEAROENIX_CORE_UI_UI_HPP
#define GEAROENIX_CORE_UI_UI_HPP

#include "../cr-types.hpp"

namespace gearoenix::core::event::ui {
enum struct Source {
    MOUSE_RIGHT,
    MOUSE_LEFT,
    MOUSE_MIDDLE,
    TOUCH,
};
struct PressData {
    Real x = 0.0f, y = 0.0f, z = 0.0f;
    Source source = Source::MOUSE_LEFT;
    std::uint8_t touch_index = 0;
};
struct RelaseData {
    Real x = 0.0f, y = 0.0f, z = 0.0f;
    Source source = Source::MOUSE_LEFT;
    std::uint8_t touch_index = 0;
};
struct ClickedData {
    Real x = 0.0f, y = 0.0f, z = 0.0f;
    Source source = Source::MOUSE_LEFT;
    std::uint8_t touch_index = 0;
};
}
#endif
