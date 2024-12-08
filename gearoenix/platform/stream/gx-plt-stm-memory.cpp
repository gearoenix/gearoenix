#include "gx-plt-stm-memory.hpp"
#include "../gx-plt-log.hpp"
#include <cstring>

gearoenix::platform::stream::Memory::Memory() = default;

gearoenix::platform::stream::Memory::~Memory() = default;

std::uint64_t gearoenix::platform::stream::Memory::read(void* d, const std::uint64_t length)
{
    const auto sz = length + index;
    const auto result = sz > mem_data.size() ? mem_data.size() - index : length;
    if (0 == result) {
        return 0;
    }
    std::memcpy(d, &(mem_data[index]), result);
    index += result;
    return result;
}

std::uint64_t gearoenix::platform::stream::Memory::write(const void* d, const std::uint64_t length)
{
    if (0 == length) {
        return 0;
    }
    const auto end = length + index;
    for (auto i = index; i < end; ++i) {
        mem_data.push_back(0);
    }
    std::memcpy(&(mem_data[index]), d, length);
    return length;
}

void gearoenix::platform::stream::Memory::seek(const std::uint64_t offset)
{
    index = offset;
}

std::uint64_t gearoenix::platform::stream::Memory::tell()
{
    return index;
}

std::uint64_t gearoenix::platform::stream::Memory::size()
{
    return mem_data.size();
}

void gearoenix::platform::stream::Memory::flush() { }

std::vector<std::uint8_t> gearoenix::platform::stream::Memory::get_file_content()
{
    return mem_data;
}
