#include "gx-gl-shd-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"

gearoenix::gl::shader::Manager::Manager(Engine& e) noexcept
    : e(e)
{
}

#endif
