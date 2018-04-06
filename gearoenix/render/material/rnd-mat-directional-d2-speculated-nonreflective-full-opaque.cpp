#include "rnd-mat-directional-d2-speculated-nonreflective-full-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

const gearoenix::core::Id gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::SHADER_ID = gearoenix::render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE;

gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

void gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources::set_texture(texture::Texture2D* t)
{
    txt = t;
}

void gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources::set_shadow_texture(texture::Texture2D* t)
{
    shdtxt = t;
}

gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::DirectionalD2SpeculatedNonreflectiveFullOpaque(system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    core::Id texid;
    f->read(texid);
    u.spec_color.read(f);
    u.spec_factors.read(f);
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    core::Count curloc = f->tell();
    t = std::static_pointer_cast<texture::Texture2D>(astmgr->get_texture(texid, core::sync::EndCaller<render::texture::Texture>([this, end, e](std::shared_ptr<render::texture::Texture> asset) -> void {
        shdrsc = static_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
        shdrsc->set_texture(static_cast<texture::Texture2D*>(asset.get()));
    })));
    f->seek(curloc);
}

gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::~DirectionalD2SpeculatedNonreflectiveFullOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::needs_mvp() const
{
    return false;
}

bool gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::update(const scene::Scene* s, const model::Model* m)
{
    const std::shared_ptr<camera::Camera>& cam = s->get_current_camera();
    const light::Sun* sun = s->get_sun();
    u.ambl_color = s->get_ambient_light();
    u.db = sun->get_bias();
    cam->get_location(u.eye);
    u.m = m->get_m();
    u.sun = sun->get_direction();
    u.sun_color = sun->get_color();
    u.vp = cam->get_view_projection();
}

void gearoenix::render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::bind(texture::Texture2D* shadow_texture)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->set_shadow_texture(shadow_texture);
    shdrsc->bind();
}
