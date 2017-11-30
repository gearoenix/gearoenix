#include "rnd-mat-directional-colored-speculated-baked-full-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

const gearoenix::core::Id gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::SHADER_ID = gearoenix::render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE;

gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::Resources::set_baked_env(texture::Cube* t)
{
    env = t;
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::Resources::set_shadow_texture(texture::Texture2D* t)
{
    shdtxt = t;
}

gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::DirectionalColoredSpeculatedBakedFullOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
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

gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::~DirectionalColoredSpeculatedBakedFullOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::needs_mvp() const
{
    return false;
}

bool gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::update(const scene::Scene* s, const model::Model* m)
{
    u.ambl_color = s->get_ambient_light() * color;
    u.db = s->get_sun()->get_bias();
    u.m = m->get_m();
    u.sun = s->get_sun()->get_direction();
    u.sun_color = s->get_sun()->get_color() * color;
    u.eye = s->get_current_camera()->get_location();
    u.vp = s->get_current_camera()->get_view_projection();
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::bind(texture::Texture2D* shadow_texture)
{
    shdrsc->set_shadow_texture(shadow_texture);
    shdrsc->bind();
}