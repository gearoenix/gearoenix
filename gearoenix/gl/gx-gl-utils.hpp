#ifndef GEAROENIX_GL_UTILS_HPP
#define GEAROENIX_GL_UTILS_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL
#include "../render/texture/gx-rnd-txt-face.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
[[nodiscard]] gl::enumerated convert(render::texture::Face f) noexcept;
}
#endif
#endif
