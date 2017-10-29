#include "rnd-mat-white.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../shader/rnd-shd-resources.hpp"

const gearoenix::core::Id gearoenix::render::material::White::SHADER_ID = gearoenix::render::shader::WHITE_POS;

gearoenix::render::material::White::White(Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    shdrsc = e->create_shader_resources(SHADER_ID, pl.get(), ub, end);
}

gearoenix::render::material::White::~White() {}

unsigned int gearoenix::render::material::White::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

void gearoenix::render::material::White::update(const scene::Scene*, const model::Model* m)
{
    u.mvp = m->get_mvp();
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::White::bind()
{
    shdrsc->bind();
}
