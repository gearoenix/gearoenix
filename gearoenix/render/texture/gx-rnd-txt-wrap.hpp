#ifndef GEAROENIX_RENDER_TEXTURE_WRAP_HPP
#define GEAROENIX_RENDER_TEXTURE_WRAP_HPP
#include "../../core/gx-cr-types.hpp"
#include <string>

namespace gearoenix::render::texture {
enum struct Wrap : core::TypeId {
    ClampToEdge = 1,
    Mirror = 2,
    Repeat = 3,
};
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::Wrap w);
}
#endif
