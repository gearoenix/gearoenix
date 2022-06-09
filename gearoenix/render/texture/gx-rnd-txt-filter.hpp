#ifndef GEAROENIX_RENDER_TEXTURE_FILTER_HPP
#define GEAROENIX_RENDER_TEXTURE_FILTER_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix::render::texture {
enum struct Filter : core::TypeId {
    Cubic = 1,
    CubicMipmapCubic = 2,
    CubicMipmapLinear = 3,
    CubicMipmapNearest = 4,
    Linear = 5,
    LinearMipmapCubic = 6,
    LinearMipmapLinear = 7,
    LinearMipmapNearest = 8,
    Nearest = 9,
    NearestMipmapCubic = 10,
    NearestMipmapLinear = 11,
    NearestMipmapNearest = 12,
};
}
#endif
