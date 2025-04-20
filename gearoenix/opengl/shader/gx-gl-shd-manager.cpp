#include "gx-gl-shd-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../gx-gl-engine.hpp"

namespace {
gearoenix::gl::shader::Manager* instance = nullptr;
}

gearoenix::gl::shader::Manager::Manager()
{
    GX_ASSERT_D(!instance);
    instance = this;
}

gearoenix::gl::shader::Manager::~Manager()
{
    GX_ASSERT_D(instance);
    instance = nullptr;
}

gearoenix::gl::shader::Manager& gearoenix::gl::shader::Manager::get()
{
    GX_ASSERT_D(instance);
    return *instance;
}

#endif
