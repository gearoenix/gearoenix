#include "rnd-mat-directional-colored-speculated-nonreflective-shadowless-opaque.hpp"
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

const gearoenix::core::Id gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::SHADER_ID = gearoenix::render::shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE;

gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    color.read(f);
    u.spec_color.read(f);
    u.spec_factors.read(f);
    shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
}

gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::~DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::needs_mvp() const
{
    return false;
}

bool gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::update(const scene::Scene* s, const model::Model* m)
{
    if (color_changed || s->get_ambient_light_changed())
        u.ambl_color = s->get_ambient_light() * color;
    u.m = m->get_m();
    const camera::Camera* cam = s->get_current_camera();
    u.vp = cam->get_view_projection();
    u.eye = cam->get_location();
    const light::Sun* sun = s->get_sun();
    u.sun = sun->get_direction();
    if (color_changed || sun->get_color_changed())
        u.sun_color = sun->get_color() * color;
    color_changed = true;
}

void gearoenix::render::material::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::bind(texture::Texture2D*)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->bind();
}
