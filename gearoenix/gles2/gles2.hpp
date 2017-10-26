#ifdef USE_SDL
#ifndef IN_IOS
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL_opengles2.h>
#endif
#elif defined(USE_GLFW)
#include <GL/glew.h>
#else
#error "Not implemented yet!"
#endif
