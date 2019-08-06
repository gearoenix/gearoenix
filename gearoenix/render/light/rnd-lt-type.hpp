#ifndef GEAROENIX_RENDER_LIGHT_TYPE_HPP
#define GEAROENIX_RENDER_LIGHT_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::light {
    enum struct Type : core::TypeId {
        CONE = 1,
        DIRECTIONAL = 2,
        POINT = 3,
    };
}
#endif
