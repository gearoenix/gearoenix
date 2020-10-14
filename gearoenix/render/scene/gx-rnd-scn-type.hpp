#ifndef GEAROENIX_RENDER_SCENE_TYPE_HPP
#define GEAROENIX_RENDER_SCENE_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
#include <ostream>

namespace gearoenix::render::scene {
enum struct Type : core::TypeId {
    Game = 1,
    Ui = 2,
};
}

std::ostream& operator<<(std::ostream&, gearoenix::render::scene::Type) noexcept;

#endif
