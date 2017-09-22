#include "vk-buf-sub-buffer.hpp"

gearoenix::render::buffer::SubBuffer::SubBuffer(unsigned int size, Buffer* buff)
    : core::gc::Object(size)
    , buff(buff)
{
}

gearoenix::render::buffer::SubBuffer::~SubBuffer()
{
}

const gearoenix::render::buffer::Buffer* gearoenix::render::buffer::SubBuffer::get_buffer() const
{
    return buff;
}
