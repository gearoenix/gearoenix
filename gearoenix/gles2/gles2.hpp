#include "../core/cr-build-configuration.hpp"
#ifdef USE_SDL
#ifdef IN_LINUX
#include <SDL2/SDL_opengles2.h>
#elif defined(IN_IOS) || defined(IN_WINDOWS)
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
#define CHECK_FOR_GRAPHIC_API_ERROR                               \
    {                                                             \
        GLenum glError = glGetError();                            \
        if (GL_NO_ERROR == glError) {                             \
        } else if (GL_INVALID_ENUM == glError) {                  \
            LOGF("Invalid enum error.");                          \
        } else if (GL_INVALID_VALUE == glError) {                 \
            LOGF("Invalid value error.");                         \
        } else if (GL_INVALID_OPERATION == glError) {             \
            LOGF("Invalid operation error.");                     \
        } else if (GL_INVALID_FRAMEBUFFER_OPERATION == glError) { \
            LOGF("Invalid framebuffer operation error.");         \
        } else if (GL_OUT_OF_MEMORY == glError) {                 \
            LOGF("Out of memory error.");                         \
        } else {                                                  \
            LOGF("Unknown error.");                               \
        }                                                         \
    }

#define GLES2_PROFILING
