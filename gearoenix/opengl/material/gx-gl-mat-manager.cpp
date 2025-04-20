#include "gx-gl-mat-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include "gx-gl-mat-pbr.hpp"
#include "gx-gl-mat-sprite.hpp"
#include "gx-gl-mat-unlit.hpp"

void gearoenix::gl::material::Manager::construct_pbr(const std::string& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    Pbr::construct(name, std::move(c));
}

void gearoenix::gl::material::Manager::construct_unlit(const std::string& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    Unlit::construct(name, std::move(c));
}

void gearoenix::gl::material::Manager::construct_sprite(const std::string& name, core::job::EndCallerShared<render::material::Sprite>&& c)
{
    Sprite::construct(name, std::move(c));
}

gearoenix::gl::material::Manager::Manager()
{
}

#endif