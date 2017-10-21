#include "gles2-engine.hpp"
#ifdef USE_OPENGL_ES2
#include "../system/sys-log.hpp"
#include <SDL2/SDL_opengles2.h>

gearoenix::gles2::Engine::Engine(system::Application* sysapp)
    : render::Engine(sysapp)
{
    TODO;
}

gearoenix::gles2::Engine::~Engine()
{
    TODO;
}

void gearoenix::gles2::Engine::window_changed()
{
    TODO;
}

void gearoenix::gles2::Engine::update()
{
    glClearColor(0.9, 0.9, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void gearoenix::gles2::Engine::terminate()
{
    TODO;
}

#endif
