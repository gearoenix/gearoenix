#ifndef GEAROENIX_RENDER_COMMAND_TYPE_HPP
#define GEAROENIX_RENDER_COMMAND_TYPE_HPP
#include "../../core/gx-cr-types.hpp"

namespace gearoenix::render::command {
enum struct Type : core::TypeId {
    Primary = 1,
    Secondary = 2,
};
}

#endif