#include "rnd-mat-skybox-basic.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

const gearoenix::core::Id gearoenix::render::material::SkyboxBasic::SHADER_ID = gearoenix::render::shader::SKYBOX_BASIC;

gearoenix::render::material::SkyboxBasic::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

void gearoenix::render::material::SkyboxBasic::Resources::set_cube_texture(texture::Cube* t)
{
    ctxt = t;
}

gearoenix::render::material::SkyboxBasic::SkyboxBasic(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    core::Id texid;
    f->read(texid);
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    ctxt = std::static_pointer_cast<texture::Cube>(astmgr->get_texture(texid, core::EndCaller<render::texture::Texture>([this, end, e](std::shared_ptr<render::texture::Texture> asset) -> void {
        shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
        shdrsc->set_cube_texture(reinterpret_cast<texture::Cube*>(asset.get()));
        (void)end;
    })));
}

gearoenix::render::material::SkyboxBasic::~SkyboxBasic()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::SkyboxBasic::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::SkyboxBasic::get_shader_id() const
{
    return SHADER_ID;
}

bool gearoenix::render::material::SkyboxBasic::needs_mvp() const
{
    return false;
}

bool gearoenix::render::material::SkyboxBasic::needs_dbm() const
{
    return false;
}

void gearoenix::render::material::SkyboxBasic::update(const scene::Scene* s, const model::Model*)
{
    const std::shared_ptr<camera::Camera>& cam = s->get_current_camera();
    math::Vec3 l;
    cam->get_location(l);
    u.vp = cam->get_view_projection() * math::Mat4x4::translator(l);
}

void gearoenix::render::material::SkyboxBasic::bind(texture::Texture2D*)
{
    ub->update(&u, sizeof(Uniform));
    shdrsc->bind();
}
