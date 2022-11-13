#include "gx-gl-mat-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include "gx-gl-mat-pbr.hpp"
#include "gx-gl-mat-unlit.hpp"

std::shared_ptr<gearoenix::render::material::Pbr> gearoenix::gl::material::Manager::construct_pbr(
    const std::string& name,
    const core::sync::EndCallerIgnored& c) noexcept
{
    return allocator.make_shared<Pbr>(gl_e, c);
}

std::shared_ptr<gearoenix::render::material::Unlit> gearoenix::gl::material::Manager::construct_unlit(
    const std::string& name,
    const core::sync::EndCallerIgnored& c) noexcept
{
    return allocator.make_shared<Unlit>(gl_e, c);
}

gearoenix::gl::material::Manager::Manager(Engine& e) noexcept
    : render::material::Manager(e)
    , gl_e(e)
{
}

#endif