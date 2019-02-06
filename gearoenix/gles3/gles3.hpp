#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#ifdef GX_USE_SDL
#ifdef GX_IN_LINUX // for sdl platforms
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>
#elif defined(IN_DESKTOP)
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#elif defined(IN_IOS) || defined(IN_ANDROID)
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif // for sdl platforms
#elif defined(USE_GLFW)
#include <GL/glew.h>
#else
#error "Not implemented yet!"
#endif
#ifdef DEBUG_MODE
#define CHECK_GLES3                                     \
    switch (glGetError()) {                             \
    case GL_NO_ERROR:                                   \
        GXLOGI("No error found.");                      \
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
#define GLES3_PROFILING
#else
#define CHECK_GLES3
#endif // debug mode
#endif // opengl es 2
