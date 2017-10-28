#include "rnd-mat-white.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../shader/rnd-shd-resources.hpp"

gearoenix::render::material::White::White(Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(shader::WHITE_POSITION, sizeof(u), e, end)
{
    shdrsc = e->create_shader_resources(shader::WHITE_POSITION, pl.get(), ub, end);
}

gearoenix::render::material::White::~White() {}

unsigned int gearoenix::render::material::White::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(shader::WHITE_POSITION);
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
