#ifndef GEAROENIX_RENDER_TEXTURE_TYPE_HPP
#define GEAROENIX_RENDER_TEXTURE_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
#include <string>

namespace gearoenix::render::texture {
enum struct Type : core::TypeId {
    Texture2D = 1,
    Texture3D = 2,
    TextureCube = 3,
    Target = 4,
    Unknown = 255,
};
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::Type t);
}
#endif
