#ifndef GEAROENIX_RENDER_MATERIAL_TRANSLUCENCY_MODE_HPP
#define GEAROENIX_RENDER_MATERIAL_TRANSLUCENCY_MODE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::material {
enum struct TranslucencyMode : core::TypeId {
    Tansparent = 1,
    Opaque = 2,
};
}
#endif
