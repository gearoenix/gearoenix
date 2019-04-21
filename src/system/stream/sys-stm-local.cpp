#include "sys-stm-local.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../sys-log.hpp"

gearoenix::system::stream::Local::Local(const std::string& name, bool writable)
    : file(name, std::ios::binary | (writable ? std::ios::out : std::ios::in))
{
    if (!file.is_open())
        GXUNEXPECTED;
}

gearoenix::system::stream::Local::~Local()
{
}

gearoenix::core::Count gearoenix::system::stream::Local::read(void* data, core::Count length)
{
    file.read((char*)data, length);
    const core::Count result = (core::Count)file.gcount();
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GXUNEXPECTED;
#endif
    file.seekp(file.tellg());
    return result;
}

gearoenix::core::Count gearoenix::system::stream::Local::write(const void* data, core::Count length)
{
    const core::Count before = (core::Count)file.tellp();
    file.write((const char*)data, length);
    const core::Count result = ((core::Count)file.tellp()) - before;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GXUNEXPECTED;
#endif
    file.seekg(file.tellp());
    return result;
}

void gearoenix::system::stream::Local::seek(core::Count offset)
{
    file.seekg(offset);
    file.seekp(offset);
}

gearoenix::core::Count gearoenix::system::stream::Local::tell()
{
    return file.tellg();
}

bool gearoenix::system::stream::Local::exist(const std::string& name)
{
    std::ifstream f(name);
    return f.good();
}
