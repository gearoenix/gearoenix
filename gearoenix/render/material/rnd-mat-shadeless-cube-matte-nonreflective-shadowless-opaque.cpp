//#include "rnd-mat-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
//#include "../../core/asset/cr-asset-manager.hpp"
//#include "../../core/sync/cr-sync-end-caller.hpp"
//#include "../../system/stream/sys-stm-stream.hpp"
//#include "../../system/sys-app.hpp"
//#include "../buffer/rnd-buf-uniform.hpp"
//#include "../camera/rnd-cmr-camera.hpp"
//#include "../light/rnd-lt-sun.hpp"
//#include "../model/rnd-mdl-model.hpp"
//#include "../pipeline/rnd-pip-pipeline.hpp"
//#include "../rnd-engine.hpp"
//#include "../scene/rnd-scn-scene.hpp"
//#include "../texture/rnd-txt-texture-cube.hpp"
//
//const gearoenix::core::Id gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::SHADER_ID = gearoenix::render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE;
//
//gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
//    : shader::Resources(e, pip, u)
//{
//}
//
//void gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources::set_cube_texture(texture::Cube* t)
//{
//    ctxt = t;
//}
//
//gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::ShadelessCubeMatteNonreflectiveShadowlessOpaque(system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
//    : Material(SHADER_ID, sizeof(u), e, end)
//{
//    core::Id texid;
//    f->read(texid);
//    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
//    core::Count curloc = f->tell();
//    ctxt = std::static_pointer_cast<texture::Cube>(astmgr->get_texture(texid, core::sync::EndCaller<render::texture::Texture>([this, end, e](std::shared_ptr<render::texture::Texture> asset) -> void {
//        shdrsc = static_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
//        shdrsc->set_cube_texture(static_cast<texture::Cube*>(asset.get()));
//        (void)end;
//    })));
//    f->seek(curloc);
//}
//
//gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::~ShadelessCubeMatteNonreflectiveShadowlessOpaque()
//{
//    delete shdrsc;
//}
//
//unsigned int gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::get_vertex_elements_count() const
//{
//    return shader::Shader::get_vertex_real_count(SHADER_ID);
//}
//
//gearoenix::core::Id gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::get_shader_id() const
//{
//    return SHADER_ID;
//}
//
//bool gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::needs_mvp() const
//{
//    return true;
//}
//
//bool gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::needs_dbm() const
//{
//    return false;
//}
//
//void gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::update(const scene::Scene*, const model::Model* m)
//{
//    u.mvp = m->get_mvp();
//    u.m = m->get_m();
//}
//
//void gearoenix::render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::bind(texture::Texture2D*)
//{
//    ub->update(&u, sizeof(Uniform));
//    shdrsc->bind();
//}
