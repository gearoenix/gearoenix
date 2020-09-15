#ifndef GEAROENIX_RENDER_SKYBOX_TYPE_HPP
#define GEAROENIX_RENDER_SKYBOX_TYPE_HPP

#include "../../core/gx-cr-types.hpp"
#include <ostream>

namespace gearoenix::render::skybox {
enum struct Type : core::TypeId {
    Cube = 1,
    Equirectangular = 2,
};
}

std::ostream& operator<<(std::ostream&, gearoenix::render::skybox::Type) noexcept;
#endif
