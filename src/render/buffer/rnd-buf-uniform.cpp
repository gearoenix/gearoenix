#include "rnd-buf-uniform.hpp"

gearoenix::render::buffer::Uniform::Uniform(const unsigned int s, const std::shared_ptr<engine::Engine>& e)
    : Buffer(s, e)
{
}

gearoenix::render::buffer::Uniform::~Uniform()
{
}
