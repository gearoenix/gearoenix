#ifndef GEAROENIX_RENDER_TEXTURE_FILTER_HPP
#define GEAROENIX_RENDER_TEXTURE_FILTER_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::texture {
class Filter {
public:
    typedef enum : core::TypeId {
        CUBIC = 1,
        CUBIC_MIPMAP_CUBIC = 2,
        CUBIC_MIPMAP_LINEAR = 3,
        CUBIC_MIPMAP_NEAREST = 4,
        LINEAR = 5,
        LINEAR_MIPMAP_CUBIC = 6,
        LINEAR_MIPMAP_LINEAR = 7,
        LINEAR_MIPMAP_NEAREST = 8,
        NEAREST = 9,
        NEAREST_MIPMAP_CUBIC = 10,
        NEAREST_MIPMAP_LINEAR = 11,
        NEAREST_MIPMAP_NEAREST = 12,
    } Id;
};
}
#endif
