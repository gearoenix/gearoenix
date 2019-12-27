#ifndef GEAROENIX_RENDER_MATERIAL_TYPE_HPP
#define GEAROENIX_RENDER_MATERIAL_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::material {
enum struct Type : core::TypeId {
    Pbr = 1,
    Unlit = 2,
    Unknown,
};
}
#endif
