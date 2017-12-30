#include "../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#ifdef USE_SDL
#ifdef IN_LINUX
#include <SDL2/SDL_opengles2.h>
#elif defined(IN_MAC)
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#elif defined(IN_IOS) || defined(IN_WEB)
#include <SDL_opengles2.h>
#endif
#elif defined(USE_GLFW)
#include <GL/glew.h>
#elif defined(IN_ANDROID)
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#else
#error "Not implemented yet!"
#endif
#ifdef DEBUG_MODE
#define CHECK_FOR_GRAPHIC_API_ERROR                     \
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
#define GLES2_PROFILING
#else
#define CHECK_FOR_GRAPHIC_API_ERROR
#endif // debug mode
#endif // opengl es 2
