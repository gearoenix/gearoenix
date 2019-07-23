#ifndef GEAROENIX_RENDER_WIDGET_ALIGNMENT_HPP
#define GEAROENIX_RENDER_WIDGET_ALIGNMENT_HPP
#include <cstdint>
namespace gearoenix::render::widget {
class Alignment {
public:
    typedef enum : std::uint8_t {
        CENTER_BOTTOM = 1,
        CENTER_MIDDLE = 2,
        CENTER_TOP = 3,
        LEFT_BOTTOM = 4,
        LEFT_MIDDLE = 5,
        LEFT_TOP = 6,
        RIGHT_BOTTOM = 7,
        RIGHT_MIDDLE = 8,
        RIGHT_TOP = 9,
    } Id;
};
}
#endif