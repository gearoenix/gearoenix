#pragma once
#include <string>

namespace gearoenix::render::material {
enum struct Transparency : unsigned char {
    Opaque = 1,
    Transparent = 2,
};

bool show_debug_gui(Transparency& inout);
}

[[nodiscard]] const std::string& to_string(gearoenix::render::material::Transparency);