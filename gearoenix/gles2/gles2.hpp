#ifdef USE_SDL
#include <SDL2/SDL_opengles2.h>
#elif defined(USE_GLFW)
#include <GL/glew.h>
#else
#error "Not implemented yet!"
#endif
