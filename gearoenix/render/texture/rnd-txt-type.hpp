#ifndef GEAROENIX_RENDER_TEXTURE_TYPE_HPP
#define GEAROENIX_RENDER_TEXTURE_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
class Type {
public:
    typedef enum : core::TypeId {
        TEXTURE_2D = 1,
        TEXTURE_3D = 2,
        CUBE = 3,
    } Id;
};
}
#endif
