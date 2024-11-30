#pragma once
#include <cstdint>

namespace gearoenix::render::widget {
enum struct Type : std::uint8_t {
    Button = 1,
    Edit = 2,
    Empty = 3,
    Text = 4,
    Modal = 5,
    Label = 6,
    ColourChooser = 7,
};
}