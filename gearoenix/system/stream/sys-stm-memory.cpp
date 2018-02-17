#include "sys-stm-memory.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../sys-log.hpp"
#include <cstring>

gearoenix::system::stream::Memory::Memory() {}

gearoenix::system::stream::Memory::~Memory() {}

gearoenix::core::Count gearoenix::system::stream::Memory::read(void* d, core::Count length)
{
    const core::Count sz = length + index;
    const core::Count result = sz > data.size() ? data.size() - index : length;
#ifdef DEBUG_MODE
    if (0 == result)
        UNEXPECTED;
#endif
    std::memcpy(d, &(data[index]), result);
    index += result;
    return result;
}

gearoenix::core::Count gearoenix::system::stream::Memory::write(const void* d, core::Count length)
{
    const core::Count sz = length + index;
    if (sz > data.size())
        data.resize(sz);
    std::memcpy(&(data[index]), d, length);
    index = sz;
    return length;
}

void gearoenix::system::stream::Memory::seek(core::Count offset)
{
#ifdef DEBUG_MODE
    if (offset > data.size())
        UNEXPECTED;
#endif
    index = offset;
}

gearoenix::core::Count gearoenix::system::stream::Memory::tell()
{
    return index;
}
