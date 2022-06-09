#ifndef GEAROENIX_RENDER_TEXTURE_BINDING_POINT_HPP
#define GEAROENIX_RENDER_TEXTURE_BINDING_POINT_HPP
#include "../../core/gx-cr-types.hpp"

namespace gearoenix::render::texture {
enum struct BindingPoint : core::TypeId {
    Unknown = 0,
    Albedo = 1,
    Normal = 2,
    Emission = 3,
    MetallicRoughness = 4,
    Occlusion = 5,
};
}
#endif
