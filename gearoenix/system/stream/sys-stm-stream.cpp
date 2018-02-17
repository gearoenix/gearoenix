#include "sys-stm-stream.hpp"

gearoenix::system::stream::Stream::Stream() {}

void gearoenix::system::stream::Stream::built_in_type_read(void* data, core::Count length)
{
    read(data, length);
}

gearoenix::system::stream::Stream::~Stream() {}
