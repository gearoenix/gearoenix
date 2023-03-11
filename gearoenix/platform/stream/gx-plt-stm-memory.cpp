#include "gx-plt-stm-memory.hpp"
#include "../../core/gx-cr-build-configuration.hpp"
#include "../gx-plt-log.hpp"
#include <cstring>

gearoenix::platform::stream::Memory::Memory() noexcept = default;

gearoenix::platform::stream::Memory::~Memory() noexcept = default;

std::size_t gearoenix::platform::stream::Memory::read(void* d, std::size_t length) noexcept
{
    const std::size_t sz = length + index;
    const std::size_t result = sz > mem_data.size() ? mem_data.size() - index : length;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    std::memcpy(d, &(mem_data[index]), result);
    index += result;
    return result;
}

std::size_t gearoenix::platform::stream::Memory::write(const void* d, std::size_t length) noexcept
{
    const std::size_t sz = length + index;
    if (sz <= mem_data.size()) {
        std::memcpy(&(mem_data[index]), d, length);
        return length;
    }
    const std::size_t li = ((std::size_t)mem_data.size()) - index;
    if (li != 0)
        std::memcpy(&(mem_data[index]), d, li);
    const char* cd = (const char*)d;
    for (std::size_t i = li; i < length; ++i)
        mem_data.push_back(cd[i]);
    index = sz;
    return length;
}

void gearoenix::platform::stream::Memory::seek(std::size_t offset) noexcept
{
#ifdef GX_DEBUG_MODE
    if (offset > mem_data.size())
        GX_UNEXPECTED;
#endif
    index = offset;
}

std::size_t gearoenix::platform::stream::Memory::tell() noexcept
{
    return index;
}

std::size_t gearoenix::platform::stream::Memory::size() noexcept
{
    return mem_data.size();
}

void gearoenix::platform::stream::Memory::flush() noexcept { }
