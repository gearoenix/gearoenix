#include "sys-stm-stream.hpp"

gearoenix::system::stream::Stream::Stream() {}

void gearoenix::system::stream::Stream::built_in_type_read(void* data, core::Count length)
{
    read(data, length);
    if (is_endian_compatible)
        return;
    std::uint8_t* c_data = static_cast<std::uint8_t*>(data);
    for (core::Count i = 0, j = length - 1; i < j; ++i, --j) {
        std::uint8_t tmp = c_data[i];
        c_data[i] = c_data[j];
        c_data[j] = tmp;
    }
}

gearoenix::system::stream::Stream::~Stream() {}

std::string gearoenix::system::stream::Stream::read_string()
{
    core::Count c;
    read(c);
    std::string s;
    s.resize(c);
    read(&(s[0]), c);
    return s;
}

bool gearoenix::system::stream::Stream::read_bool()
{
    std::uint8_t data;
    read(&data, 1);
    return data != 0;
}
