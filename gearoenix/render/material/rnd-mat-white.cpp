#include "rnd-mat-white.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../shader/rnd-shd-resources.hpp"

gearoenix::render::material::White::White(Engine* e)
    : Material(shader::WHITE, sizeof(u), e)
{
    shdrsc = new shader::Resources(e, pl.get(), ub);
}

gearoenix::render::material::White::~White() {}

unsigned int gearoenix::render::material::White::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(shader::WHITE);
}

void gearoenix::render::material::White::update(const scene::Scene*, const model::Model* m)
{
    u.mvp = m->get_mvp();
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::White::bind()
{
    shdrsc->bind(pl.get());
}
