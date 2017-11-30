#include "rnd-mat-directional-colored-speculated-baked-shadowless-opaque.hpp"
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
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources::set_baked_env(texture::Cube* t)
{
    env = t;
}

gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::DirectionalColoredSpeculatedBakedShadowlessOpaque(core::Id shdid, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(shdid, sizeof(u), e, end)
    , SHADER_ID(shdid)
{
    color.read(f);
    u.spec_color.read(f);
    u.spec_factors.read(f);
    f->read(u.rfl_fac);
    core::Id texid;
    f->read(texid);
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    std::function<void()> fun = [this, end, e] {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
        shdrsc->set_baked_env(env.get());
    };
    unsigned int curloc = f->tell();
    env = std::static_pointer_cast<texture::Cube>(astmgr->get_texture(texid, core::EndCaller::create(fun)));
    f->seek(curloc);
}

gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::~DirectionalColoredSpeculatedBakedShadowlessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::needs_mvp() const
{
    return false;
}

bool gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::update(const scene::Scene* s, const model::Model* m)
{
    if (color_changed || s->get_ambient_light_changed())
        u.ambl_color = s->get_ambient_light() * color;
    u.m = m->get_m();
    const light::Sun* sun = s->get_sun();
    u.sun = sun->get_direction();
    if (color_changed || sun->get_color_changed())
        u.sun_color = sun->get_color() * color;
    const camera::Camera* cam = s->get_current_camera();
    u.eye = cam->get_location();
    u.vp = cam->get_view_projection();
    color_changed = false;
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedShadowlessOpaque::bind(texture::Texture2D*)
{
    shdrsc->bind();
}
