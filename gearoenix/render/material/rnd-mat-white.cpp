#include "rnd-mat-white.hpp"

gearoenix::render::material::White::White(Engine* e)
    : Material(shader::WHITE, sizeof(u), e)
{
}

gearoenix::render::material::White::~White() {}

unsigned int gearoenix::render::material::White::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(shader::WHITE);
}
