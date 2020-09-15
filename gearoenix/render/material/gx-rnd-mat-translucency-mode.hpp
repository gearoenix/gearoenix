#ifndef GEAROENIX_RENDER_MATERIAL_TRANSLUCENCY_MODE_HPP
#define GEAROENIX_RENDER_MATERIAL_TRANSLUCENCY_MODE_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix::render::material {
enum struct TranslucencyMode : core::TypeId {
    Transparent = 1,
    Opaque = 2,
};
}
#endif
