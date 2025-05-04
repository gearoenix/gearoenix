#include "gx-gl-shd-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"

gearoenix::gl::shader::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::gl::shader::Manager::~Manager() = default;

#endif