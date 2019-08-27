#ifndef GEAROENIX_RENDER_ENGINE_TYPE_HPP
#define GEAROENIX_RENDER_ENGINE_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::engine {
enum struct Type : core::Id {
    None = 0,
    VULKAN = 1,
    OPENGL_43 = 2,
    OPENGL_33 = 3,
    OPENGL_ES3 = 4,
    OPENGL_ES2 = 5,
    DIRECTX_11 = 6,
    DIRECTX_12 = 7,
    METAL = 8,
};
}
#endif
