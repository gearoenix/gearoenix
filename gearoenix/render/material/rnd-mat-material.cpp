#include "rnd-mat-material.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "rnd-mat-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-directional-colored-speculated-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-directional-textured-speculated-nonreflective-full-opaque.hpp"
#include "rnd-mat-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-white.hpp"

gearoenix::render::material::Material::Material(core::Id sid, unsigned int us, Engine* e, std::shared_ptr<core::EndCaller> end)
    : ub(e->create_uniform(us, end))
    , e(e)
{
    system::File* f = e->get_system_application()->get_asset_manager()->get_file();
    unsigned int curloc = f->tell();
    pl = e->get_pipeline_manager()->get_pipeline(sid, end);
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
    case shader::WHITE_POS:
        TODO;
        return new White(e, end);
    case shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new ShadelessColoredMatteNonreflectiveShadowlessOpaque(f, e, end);
    case shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new DirectionalColoredMatteNonreflectiveShadowlessOpaque(f, e, end);
    case shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(f, e, end);
    case shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new ShadelessCubeMatteNonreflectiveShadowlessOpaque(f, e, end);
    default:
        UNIMPLEMENTED;
    }
}
