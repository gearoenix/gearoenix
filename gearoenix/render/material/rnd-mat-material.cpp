#include "rnd-mat-material.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "rnd-mat-depth.hpp"
#include "rnd-mat-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-directional-colored-speculated-baked-full-opaque.hpp"
#include "rnd-mat-directional-colored-speculated-baked-shadowless-opaque.hpp"
#include "rnd-mat-directional-colored-speculated-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-directional-d2-speculated-baked-full-opaque.hpp"
#include "rnd-mat-directional-d2-speculated-nonreflective-full-opaque.hpp"
#include "rnd-mat-directional-d2-speculated-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#include "rnd-mat-skybox-basic.hpp"

gearoenix::render::material::Material::Material(core::Id sid, unsigned int us, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
    : ub(e->create_uniform(us, end))
    , e(e)
{
    system::stream::Stream* f = e->get_system_application()->get_asset_manager()->get_file();
    core::Count curloc = f->tell();
    pl = e->get_pipeline_manager()->get_pipeline(sid, end);
    f->seek(curloc);
}

gearoenix::render::material::Material::~Material()
{
    delete ub;
}

gearoenix::render::material::Material* gearoenix::render::material::Material::read(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
{
    shader::Id sid;
    f->read(sid);
    //    LOGE("location: " << f->tell());
    switch (sid) {
    case shader::DEPTH_POS:
    case shader::DEPTH_POS_NRM:
    case shader::DEPTH_POS_NRM_UV:
    case shader::DEPTH_POS_UV:
        return new Depth(sid, e, end);
    case shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new DirectionalColoredMatteNonreflectiveShadowlessOpaque(f, e, end);
    case shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_CASTER_OPAQUE:
    case shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE:
        return new DirectionalColoredSpeculatedBakedShadowlessOpaque(sid, f, e, end);
    case shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
        return new DirectionalColoredSpeculatedBakedFullOpaque(f, e, end);
    case shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(f, e, end);
    case shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
        return new DirectionalD2SpeculatedBakedFullOpaque(f, e, end);
    case shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
        return new DirectionalD2SpeculatedNonreflectiveFullOpaque(f, e, end);
    case shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(f, e, end);
    case shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new ShadelessColoredMatteNonreflectiveShadowlessOpaque(sid, f, e, end);
    case shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new ShadelessCubeMatteNonreflectiveShadowlessOpaque(f, e, end);
    case shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new ShadelessD2MatteNonreflectiveShadowlessOpaque(sid, f, e, end);
    case shader::SKYBOX_BASIC:
        return new SkyboxBasic(f, e, end);
    default:
        UNIMPLEMENTED;
    }
}
