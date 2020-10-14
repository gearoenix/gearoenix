#ifndef GEAROENIX_RENDER_MODEL_TYPE_HPP
#define GEAROENIX_RENDER_MODEL_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix::render::model {
enum struct Type : core::TypeId {
    Dynamic = 1,
    Static = 2,
    Widget = 3,
};
}
#endif
