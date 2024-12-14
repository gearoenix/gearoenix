#include "gx-plt-stm-memory.hpp"
#include "../gx-plt-log.hpp"
#include <cstring>

gearoenix::platform::stream::Memory::Memory() = default;

gearoenix::platform::stream::Memory::~Memory() = default;

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Memory::read(void* d, const stream_size_t length)
{
    const auto sz = length + index;
    const auto result = sz > mem_data.size() ? mem_data.size() - index : length;
    if (0 == result) {
        return 0;
    }
    std::memcpy(d, &mem_data[index], result);
    index += result;
    return result;
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Memory::write(const void* d, const stream_size_t length)
{
    if (0 == length) {
        return 0;
    }
    auto end = length + index;
    end = end > mem_data.size() ? end - mem_data.size() : 0;
    for (auto i = 0; i < end; ++i) {
        mem_data.push_back(0);
    }
    std::memcpy(&mem_data[index], d, length);
    index += length;
    return length;
}

void gearoenix::platform::stream::Memory::seek(const stream_size_t offset)
{
    index = offset;
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Memory::tell()
{
    return index;
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Memory::size()
{
    return mem_data.size();
}

void gearoenix::platform::stream::Memory::flush() { }

std::vector<std::uint8_t> gearoenix::platform::stream::Memory::get_file_content()
{
    return mem_data;
}
