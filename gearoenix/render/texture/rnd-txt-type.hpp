#ifndef GEAROENIX_RENDER_TEXTURE_TYPE_HPP
#define GEAROENIX_RENDER_TEXTURE_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
enum struct Type : core::TypeId {
    Texture2D = 1,
    Texture3D = 2,
    TextureCube = 3,
    Target = 4,
};
}
#endif
