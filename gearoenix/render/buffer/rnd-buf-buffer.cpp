#include "rnd-buf-buffer.hpp"

gearoenix::render::buffer::Buffer::Buffer(const unsigned int size, const std::shared_ptr<engine::Engine>& e)
    : e(e)
    , size(size)
{
}

gearoenix::render::buffer::Buffer::~Buffer() {}

unsigned int gearoenix::render::buffer::Buffer::get_size() const
{
    return size;
}