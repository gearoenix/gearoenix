#ifndef GEAROENIX_RENDER_TEXTURE_FACE_HPP
#define GEAROENIX_RENDER_TEXTURE_FACE_HPP
#include "../../core/gx-cr-types.hpp"
#include <string>

namespace gearoenix::render::texture {
enum struct Face : core::TypeId {
    PositiveX = 1,
    NegativeX = 2,
    PositiveY = 3,
    NegativeY = 4,
    PositiveZ = 5,
    NegativeZ = 6,
};

constexpr Face FACES[6] = {
    Face::PositiveX,
    Face::NegativeX,
    Face::PositiveY,
    Face::NegativeY,
    Face::PositiveZ,
    Face::NegativeZ,
};
}

namespace std {
[[nodiscard]] std::string to_string(const gearoenix::render::texture::Face f) noexcept;
}

#endif
