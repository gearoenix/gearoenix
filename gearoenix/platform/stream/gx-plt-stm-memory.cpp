#include "gx-plt-stm-memory.hpp"
#include "../../core/gx-cr-build-configuration.hpp"
#include "../gx-plt-log.hpp"
#include <cstring>

gearoenix::platform::stream::Memory::Memory() noexcept = default;

gearoenix::platform::stream::Memory::~Memory() noexcept = default;

gearoenix::core::Count gearoenix::platform::stream::Memory::read(void* d, core::Count length) noexcept
{
    const core::Count sz = length + index;
    const core::Count result = sz > mem_data.size() ? mem_data.size() - index : length;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    std::memcpy(d, &(mem_data[index]), result);
    index += result;
    return result;
}

gearoenix::core::Count gearoenix::platform::stream::Memory::write(const void* d, core::Count length) noexcept
{
    const core::Count sz = length + index;
    if (sz <= mem_data.size()) {
        std::memcpy(&(mem_data[index]), d, length);
        return length;
    }
    const core::Count li = ((core::Count)mem_data.size()) - index;
    if (li != 0)
        std::memcpy(&(mem_data[index]), d, li);
    const char* cd = (const char*)d;
    for (core::Count i = li; i < length; ++i)
        mem_data.push_back(cd[i]);
    index = sz;
    return length;
}

void gearoenix::platform::stream::Memory::seek(core::Count offset) noexcept
{
#ifdef GX_DEBUG_MODE
    if (offset > mem_data.size())
        GX_UNEXPECTED;
#endif
    index = offset;
}

gearoenix::core::Count gearoenix::platform::stream::Memory::tell() noexcept
{
    return index;
}

gearoenix::core::Count gearoenix::platform::stream::Memory::size() noexcept
{
    return mem_data.size();
}
