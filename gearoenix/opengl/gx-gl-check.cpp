#include "gx-gl-check.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../platform/gx-plt-log.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"

void gearoenix::gl::check_for_error() noexcept
{
    switch (glGetError()) {
    case GL_NO_ERROR:
        break;
    case GL_INVALID_ENUM:
        GX_LOG_F("Invalid enum")
    case GL_INVALID_VALUE:
        GX_LOG_F("Invalid value")
    case GL_INVALID_OPERATION:
        GX_LOG_F("Invalid operation")
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        GX_LOG_F("Invalid framebuffer operation error.")
    case GL_OUT_OF_MEMORY:
        GX_LOG_F("Out of memory error.")
    default:
        GX_LOG_F("Unknown error.")
    }
}

#endif