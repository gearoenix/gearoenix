#include "rnd-mat-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

const gearoenix::core::Id gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::SHADER_ID = gearoenix::render::shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE;

gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::DirectionalColoredMatteNonreflectiveShadowlessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    u.color.read(f);
    shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
}

gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::~DirectionalColoredMatteNonreflectiveShadowlessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

void gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::update(const scene::Scene* s, const model::Model* m)
{
    u.ambl_color = s->get_ambient_light();
    u.m = m->get_m();
    u.mvp = m->get_mvp();
    u.sun = s->get_sun()->get_direction();
    u.sun_color = s->get_sun()->get_color();
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::bind()
{
    shdrsc->bind();
}
