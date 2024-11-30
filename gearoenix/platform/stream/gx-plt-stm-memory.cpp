#include "gx-plt-stm-memory.hpp"
#include "../../core/gx-cr-build-configuration.hpp"
#include "../gx-plt-log.hpp"
#include <cstring>

gearoenix::platform::stream::Memory::Memory() = default;

gearoenix::platform::stream::Memory::~Memory() = default;

std::uint64_t gearoenix::platform::stream::Memory::read(void* d, std::uint64_t length)
{
    const std::uint64_t sz = length + index;
    const std::uint64_t result = sz > mem_data.size() ? mem_data.size() - index : length;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    std::memcpy(d, &(mem_data[index]), result);
    index += result;
    return result;
}

std::uint64_t gearoenix::platform::stream::Memory::write(const void* d, std::uint64_t length)
{
    const std::uint64_t sz = length + index;
    if (sz <= mem_data.size()) {
        std::memcpy(&(mem_data[index]), d, length);
        return length;
    }
    const std::uint64_t li = ((std::uint64_t)mem_data.size()) - index;
    if (li != 0)
        std::memcpy(&(mem_data[index]), d, li);
    const char* cd = (const char*)d;
    for (std::uint64_t i = li; i < length; ++i)
        mem_data.push_back(cd[i]);
    index = sz;
    return length;
}

void gearoenix::platform::stream::Memory::seek(std::uint64_t offset)
{
#ifdef GX_DEBUG_MODE
    if (offset > mem_data.size())
        GX_UNEXPECTED;
#endif
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
