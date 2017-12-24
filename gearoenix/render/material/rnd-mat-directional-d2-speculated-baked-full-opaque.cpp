#include "rnd-mat-directional-d2-speculated-baked-full-opaque.hpp"
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

const gearoenix::core::Id gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::SHADER_ID = gearoenix::render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE;

gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

void gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::Resources::set_texture(texture::Texture2D* t)
{
    txt = t;
}

void gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::Resources::set_baked_env(texture::Cube* t)
{
    env = t;
}

void gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::Resources::set_shadow_texture(texture::Texture2D* t)
{
    shdtxt = t;
}

gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::DirectionalD2SpeculatedBakedFullOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    core::Id tex2did;
    f->read(tex2did);
    u.spec_color.read(f);
    u.spec_factors.read(f);
    f->read(u.rfl_fac);
    core::Id texid;
    f->read(texid);
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    std::function<void()> fun = [this, end, e] {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
        shdrsc->set_baked_env(env.get());
        shdrsc->set_texture(txt.get());
    };
    std::shared_ptr<core::EndCaller> end_caller(new core::EndCaller(fun));
    unsigned int curloc = f->tell();
    txt = std::static_pointer_cast<texture::Texture2D>(astmgr->get_texture(tex2did, end_caller));
    env = std::static_pointer_cast<texture::Cube>(astmgr->get_texture(texid, end_caller));
    f->seek(curloc);
}

gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::~DirectionalD2SpeculatedBakedFullOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::needs_mvp() const
{
    return false;
}

bool gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::update(const scene::Scene* s, const model::Model* m)
{
    u.ambl_color = s->get_ambient_light();
    const light::Sun* sun = s->get_sun();
    u.db = sun->get_bias();
    const camera::Camera* cam = s->get_current_camera();
    u.eye = cam->get_location();
    u.m = m->get_m();
    u.sun = sun->get_direction();
    u.sun_color = sun->get_color();
    u.vp = cam->get_view_projection();
}

void gearoenix::render::material::DirectionalD2SpeculatedBakedFullOpaque::bind(texture::Texture2D* shadow_texture)
{
	ub->update(&u, sizeof(Uniform));
    shdrsc->set_shadow_texture(shadow_texture);
    shdrsc->bind();
}
