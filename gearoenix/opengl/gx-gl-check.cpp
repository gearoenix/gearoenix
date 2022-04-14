#include "gx-gl-check.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../platform/gx-plt-log.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"

bool gearoenix::gl::check_for_error() noexcept
{
    switch (glGetError()) {
    case GL_NO_ERROR:
        return true;
    case GL_INVALID_ENUM:
        GX_LOG_E("Invalid enum");
        return false;
    case GL_INVALID_VALUE:
        GX_LOG_E("Invalid value");
        return false;
    case GL_INVALID_OPERATION:
        GX_LOG_E("Invalid operation");
        return false;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        GX_LOG_E("Invalid framebuffer operation error.");
        return false;
    case GL_OUT_OF_MEMORY:
        GX_LOG_E("Out of memory error.");
        return false;
    default:
        GX_LOG_E("Unknown error.");
        return false;
    }
}

#endif