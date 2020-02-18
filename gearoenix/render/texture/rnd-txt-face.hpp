#ifndef GEAROENIX_RENDER_TEXTURE_FACE_HPP
#define GEAROENIX_RENDER_TEXTURE_FACE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
enum struct Face : core::TypeId {
    POSITIVE_X = 1,
    NEGATIVE_X = 2,
    POSITIVE_Y = 3,
    NEGATIVE_Y = 4,
    POSITIVE_Z = 5,
    NEGATIVE_Z = 6,
};
}
#endif
