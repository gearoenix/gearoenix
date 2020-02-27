#ifndef GEAROENIX_GL_UTILS_HPP
#define GEAROENIX_GL_UTILS_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL
#include "../render/texture/rnd-txt-face.hpp"
#include "gl-types.hpp"

namespace gearoenix::gl {
[[nodiscard]] gl::enumerated convert(render::texture::Face f) noexcept;
}
#endif
#endif
