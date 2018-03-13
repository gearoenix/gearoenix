#include "rnd-mat-directional-colored-speculated-baked-full-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
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

gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::DirectionalColoredSpeculatedBakedFullOpaque(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    color.read(f);
    u.ambl_color.w(color.w());
    u.sun_color.w(color.w());
    u.spec_color.read(f);
    u.spec_factors.read(f);
    f->read(u.rfl_fac);
    core::Id texid;
    f->read(texid);
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    core::Count curloc = f->tell();
    env = std::static_pointer_cast<texture::Cube>(astmgr->get_texture(texid, core::EndCaller<render::texture::Texture>([this, end, e](std::shared_ptr<render::texture::Texture> asset) -> void {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
        shdrsc->set_baked_env(reinterpret_cast<texture::Cube*>(asset.get()));
    })));
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
    if (color_changed || s->get_ambient_light_changed())
        u.ambl_color.xyz(s->get_ambient_light() * color.xyz());
    const light::Sun* sun = s->get_sun();
    u.db = sun->get_bias();
    u.m = m->get_m();
    u.sun = sun->get_direction();
    if (color_changed || sun->get_color_changed())
        u.sun_color.xyz(sun->get_color() * color.xyz());
    const camera::Camera* cam = s->get_current_camera();
    cam->get_location(u.eye);
    u.vp = cam->get_view_projection();
    color_changed = false;
}

void gearoenix::render::material::DirectionalColoredSpeculatedBakedFullOpaque::bind(texture::Texture2D* shadow_texture)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->set_shadow_texture(shadow_texture);
    shdrsc->bind();
}
