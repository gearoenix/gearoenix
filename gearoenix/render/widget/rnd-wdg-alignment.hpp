#ifndef GEAROENIX_RENDER_WIDGET_ALIGNMENT_HPP
#define GEAROENIX_RENDER_WIDGET_ALIGNMENT_HPP
#include <cstdint>
namespace gearoenix::render::widget {
enum struct Alignment : std::uint8_t {
    Center = 1,
    Start = 2,
    End = 3,
};
}
#endif