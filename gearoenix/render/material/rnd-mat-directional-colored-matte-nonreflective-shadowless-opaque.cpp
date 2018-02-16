#include "rnd-mat-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
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

gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::DirectionalColoredMatteNonreflectiveShadowlessOpaque(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    color.read(f);
    u.ambl_color.w(color.w());
    u.sun_color.w(color.w());
    e->add_load_function([this, e, end]() -> void {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
    });
}

gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::~DirectionalColoredMatteNonreflectiveShadowlessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::get_shader_id() const
{
    return SHADER_ID;
}
bool gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::needs_mvp() const
{
    return true;
}

bool gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::update(const scene::Scene* s, const model::Model* m)
{
    if (s->get_ambient_light_changed() || color_changed) {
        u.ambl_color.xyz(s->get_ambient_light() * color.xyz());
    }
    u.m = m->get_m();
    u.mvp = m->get_mvp();
    const light::Sun* sun = s->get_sun();
    u.sun = sun->get_direction();
    if (color_changed || sun->get_color_changed()) {
        u.sun_color.xyz(sun->get_color() * color.xyz());
    }
    color_changed = false;
}

void gearoenix::render::material::DirectionalColoredMatteNonreflectiveShadowlessOpaque::bind(texture::Texture2D*)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->bind();
}
