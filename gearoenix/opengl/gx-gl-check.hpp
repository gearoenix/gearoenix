#ifndef GEAROENIX_GL_CHECK_HPP
#define GEAROENIX_GL_CHECK_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/gx-cr-build-configuration.hpp"

#ifdef GX_DEBUG_MODE
#define GX_GL_CHECK_D check_for_error()
#else
#define GX_GL_CHECK_D static_assert(true, "")
#endif

namespace gearoenix::gl {
void check_for_error() noexcept;
}

#endif
#endif