#ifndef GEAROENIX_RENDER_TRANSLUCENCY_MODE_HPP
#define GEAROENIX_RENDER_TRANSLUCENCY_MODE_HPP
#include "../core/gx-cr-types.hpp"

namespace gearoenix::render {
enum struct TranslucencyMode : core::TypeId {
    Transparent = 1,
    Opaque = 2,
};
}
#endif
