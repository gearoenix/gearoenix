#include "rnd-mat-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/file/sys-fl-file.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

void gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources::set_texture_2d(texture::Texture2D* t2d)
{
    txt2d = t2d;
}

gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::ShadelessD2MatteNonreflectiveShadowlessOpaque(core::Id sid, system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
    : Material(sid, sizeof(u), e, end)
    , SHADER_ID(sid)
{
    core::Id texid;
    f->read(texid);
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    unsigned int curloc = f->tell();
    txt2d = std::static_pointer_cast<texture::Texture2D>(astmgr->get_texture(texid, core::EndCaller<texture::Texture>([this, end, e](std::shared_ptr<texture::Texture> asset) -> void {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
        shdrsc->set_texture_2d(reinterpret_cast<texture::Texture2D*>(asset.get()));
    })));
    f->seek(curloc);
}

gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::~ShadelessD2MatteNonreflectiveShadowlessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::needs_mvp() const
{
    return true;
}

bool gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::update(const scene::Scene*, const model::Model* m)
{
    u.mvp = m->get_mvp();
}

void gearoenix::render::material::ShadelessD2MatteNonreflectiveShadowlessOpaque::bind(texture::Texture2D*)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->bind();
}
