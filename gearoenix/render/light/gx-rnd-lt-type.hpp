#ifndef GEAROENIX_RENDER_LIGHT_TYPE_HPP
#define GEAROENIX_RENDER_LIGHT_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix::render::light {
enum struct Type : core::TypeId {
    Cone = 1,
    Directional = 2,
    Point = 3,
};
}
#endif
