#include "rnd-mat-depth.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../shader/rnd-shd-resources.hpp"

gearoenix::render::material::Depth::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

gearoenix::render::material::Depth::Depth(core::Id shdid, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Material(shdid, sizeof(u), e, end)
    , SHADER_ID(shdid)
{
    e->add_load_function([this, e, end]() -> void {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
    });
}

gearoenix::render::material::Depth::~Depth()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::Depth::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::Depth::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::Depth::needs_mvp() const
{
    return true;
}

bool gearoenix::render::material::Depth::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::Depth::update(const scene::Scene*, const model::Model* m)
{
    u.mvp = m->get_sun_mvp();
}

void gearoenix::render::material::Depth::bind(texture::Texture2D*)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->bind();
}
