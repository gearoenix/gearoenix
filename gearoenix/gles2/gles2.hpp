#ifdef USE_SDL
#include <SDL2/SDL_opengles2.h>
#elif defined(USE_GLFW)
#include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#error "Not implemented yet!"
#endif
