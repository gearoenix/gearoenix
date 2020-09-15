#ifndef GEAROENIX_RENDER_REFLECTION_TYPE_HPP
#define GEAROENIX_RENDER_REFLECTION_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
#include <ostream>

namespace gearoenix::render::reflection {
enum struct Type : core::TypeId {
    Baked = 1,
    Runtime = 2,
};
std::ostream& operator<<(std::ostream& os, Type t) noexcept;
}

#endif
