#include "rnd-mat-depth.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../shader/rnd-shd-resources.hpp"

const gearoenix::core::Id gearoenix::render::material::Depth::SHADER_ID = gearoenix::render::shader::DEPTH_POS;

gearoenix::render::material::Depth::Depth(Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    shdrsc = e->create_shader_resources(SHADER_ID, pl.get(), ub, end);
}

gearoenix::render::material::Depth::~Depth() {}

unsigned int gearoenix::render::material::Depth::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

gearoenix::core::Id gearoenix::render::material::Depth::get_shader_id() const
{
    return SHADER_ID;
}

void gearoenix::render::material::Depth::update(const scene::Scene*, const model::Model* m)
{
    u.mvp = m->get_mvp();
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::Depth::bind(texture::Texture2D*)
{
    shdrsc->bind();
}
