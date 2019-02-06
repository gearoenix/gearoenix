#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2

// Declaring gl data
#if defined(GX_IN_DESKTOP)
#define GLEW_STATIC 
#include <GL/glew.h>
#include <SDL_opengl.h>
#elif defined(GX_IN_IOS) || defined(GX_IN_WEB) || defined(GX_IN_ANDROID)
#include <SDL_opengles2.h>
#else
#error "Unexpected platform! Other platforms are not implemented yet."
#endif

#ifdef GX_DEBUG_MODE
#define GX_DEBUG_GLES2
#endif

#ifdef GX_DEBUG_GLES2
#define GX_CHECK_FOR_GRAPHIC_API_ERROR                  \
    switch (glGetError()) {                             \
    case GL_NO_ERROR:                                   \
        break;                                          \
    case GL_INVALID_ENUM:                               \
        GXLOGF("Invalid enum");                         \
    case GL_INVALID_VALUE:                              \
        GXLOGF("Invalid value");                        \
    case GL_INVALID_OPERATION:                          \
        GXLOGF("Invalid operation");                    \
    case GL_INVALID_FRAMEBUFFER_OPERATION:              \
        GXLOGF("Invalid framebuffer operation error."); \
    case GL_OUT_OF_MEMORY:                              \
        GXLOGF("Out of memory error.");                 \
    default:                                            \
        GXLOGF("Unknown error.");                       \
    }
#else
#define GX_CHECK_FOR_GRAPHIC_API_ERROR
#endif

#define GX_GLES2_PROFILING

#endif // opengl es 2
