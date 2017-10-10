#include "vk-buf-sub-buffer.hpp"
#include "../../system/sys-log.hpp"
#include "../command/vk-cmd-buffer.hpp"
#include "vk-buf-buffer.hpp"

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

void gearoenix::render::buffer::SubBuffer::write(const void* data, unsigned int data_size)
{
#ifdef DEBUG_MODE
    if (data_size > size) {
        LOGF("Data size (" << data_size << ") is bigger than buffer size (" << size << ")");
    }
#endif
    buff->write(data, data_size, offset);
}

void gearoenix::render::buffer::SubBuffer::copy_from(command::Buffer* cb, SubBuffer* sb)
{
    VkBufferCopy bc;
    bc.dstOffset = offset;
    bc.size = size;
    bc.srcOffset = sb->offset;
    cb->copy_buffer(sb->buff->get_vulkan_data(), buff->get_vulkan_data(), bc);
}
