#include "rnd-mat-material.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "rnd-mat-directional-textured-speculated-nocube-fullshadow-opaque.hpp"
#include "rnd-mat-white.hpp"

gearoenix::render::material::Material::Material(shader::Id sid, unsigned int us, Engine* e)
    : ub(new buffer::Uniform(us, e))
    , e(e)
{
    system::File* f = e->get_system_application()->get_asset_manager()->get_file();
    unsigned int curloc = f->tell();
    pl = e->get_pipeline_manager()->get_pipeline(sid);
    f->seek(curloc);
}

gearoenix::render::material::Material::~Material()
{
    delete ub;
}

gearoenix::render::material::Material* gearoenix::render::material::Material::read(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
{
    shader::Id sid;
    f->read(sid);
    //    LOGE("location: " << f->tell());
    switch (sid) {
    case shader::WHITE:
        return new White(e);
    case shader::DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        //        LOGE("location: " << f->tell());
        return new DirectionalTexturedSpeculatedNocubeFullshadowOpaque(f, e, end);
    }
    LOGF("Unexpected");
}
