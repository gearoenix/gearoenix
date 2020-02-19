#ifndef GEAROENIX_RENDER_TEXTURE_FACE_HPP
#define GEAROENIX_RENDER_TEXTURE_FACE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
enum struct Face : core::TypeId {
    PositiveX = 1,
    NegativeX = 2,
    PositiveY = 3,
    NegativeY = 4,
    PositiveZ = 5,
    NegativeZ = 6,
};
}
#endif
