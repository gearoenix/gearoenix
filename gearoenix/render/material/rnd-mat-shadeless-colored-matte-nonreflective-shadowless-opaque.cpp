#include "rnd-mat-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
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

gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::ShadelessColoredMatteNonreflectiveShadowlessOpaque(core::Id sid, system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
    : Material(sid, sizeof(u), e, end)
    , SHADER_ID(sid)
{
    u.color.read(f);
    e->add_load_function([this, e, end]() -> void {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
    });
}

gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::~ShadelessColoredMatteNonreflectiveShadowlessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::needs_mvp() const
{
    return true;
}

bool gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::update(const scene::Scene*, const model::Model* m)
{
    u.mvp = m->get_mvp();
}

void gearoenix::render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::bind(texture::Texture2D*)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->bind();
}
