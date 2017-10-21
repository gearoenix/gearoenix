#include "gl-engine.hpp"
#ifdef USE_OPENGL
#include "../system/sys-log.hpp"
#include <GL/glew.h>

gearoenix::opengl4::Engine::Engine(system::Application* sysapp)
    : render::Engine(sysapp)
{
    TODO;
}

gearoenix::opengl4::Engine::~Engine()
{
    TODO;
}

void gearoenix::opengl4::Engine::window_changed()
{
    TODO;
}

void gearoenix::opengl4::Engine::update()
{
    glClearColor(0.9, 0.9, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void gearoenix::opengl4::Engine::terminate()
{
    TODO;
}

bool gearoenix::opengl4::Engine::is_supported()
{
    TODO;
    return true;
}

#endif
