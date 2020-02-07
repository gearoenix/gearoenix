#ifndef GEAROENIX_RENDER_TEXTURE_WRAP_HPP
#define GEAROENIX_RENDER_TEXTURE_WRAP_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
enum struct Wrap : core::TypeId {
    CLAMP_TO_EDGE = 1,
    MIRROR = 2,
    REPEAT = 3,
};
}
#endif
