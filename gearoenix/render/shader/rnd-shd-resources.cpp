#include "rnd-shd-resources.hpp"

gearoenix::render::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u)
    : e(e)
    , pip(p)
    , u(u)
{
}

gearoenix::render::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u, texture::Texture2D* t)
    : e(e)
    , pip(p)
    , u(u)
    , t(t)
{
}

gearoenix::render::shader::Resources::~Resources()
{
}
