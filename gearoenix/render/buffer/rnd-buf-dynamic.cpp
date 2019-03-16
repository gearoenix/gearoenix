#include "rnd-buf-dynamic.hpp"

gearoenix::render::buffer::Dynamic::Dynamic(const unsigned int s, const std::shared_ptr<engine::Engine>& e) : Buffer(s, e) {}

gearoenix::render::buffer::Dynamic::~Dynamic()
{
}

