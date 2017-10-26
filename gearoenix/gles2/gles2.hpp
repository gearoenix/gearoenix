#ifdef USE_SDL
#ifndef IN_MAC
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#elif defined(USE_GLFW)
#include <GL/glew.h>
#else
#error "Not implemented yet!"
#endif
