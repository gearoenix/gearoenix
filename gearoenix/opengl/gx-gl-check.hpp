#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../platform/gx-plt-build-configuration.hpp"

#define GX_GL_CHECK_ENABLED GX_DEBUG_MODE && !GX_PLATFORM_ANDROID

#if GX_GL_CHECK_ENABLED
#define GX_GL_CHECK_D GX_ASSERT_D(check_for_error())

namespace gearoenix::gl {
void ignore_gl_errors();
}

#else

#define GX_GL_CHECK_D static_assert(true, "")
#define ignore_gl_errors() static_assert(true, "")

#endif

namespace gearoenix::gl {
[[nodiscard]] bool check_for_error();
}

#endif