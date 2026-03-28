#include "gx-gl-mat-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/gx-cr-object.hpp"
#include "../gx-gl-engine.hpp"
#include "gx-gl-mat-pbr.hpp"
#include "gx-gl-mat-sprite.hpp"
#include "gx-gl-mat-unlit.hpp"

void gearoenix::gl::material::Manager::construct_pbr(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    Pbr::construct(std::move(name), std::move(c));
}

void gearoenix::gl::material::Manager::construct_unlit(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    Unlit::construct(std::move(name), std::move(c));
}

void gearoenix::gl::material::Manager::construct_sprite(std::string&& name, core::job::EndCallerShared<render::material::Sprite>&& c)
{
    Sprite::construct(std::move(name), std::move(c));
}

gearoenix::gl::material::Manager::Manager()
{
    core::Object::register_type<Material, Pbr>();

    core::Object::register_type<Pbr>();
    core::Object::register_type<Sprite>();
    core::Object::register_type<Unlit>();
}

#endif