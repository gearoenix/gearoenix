#include "sys-stm-local.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../sys-log.hpp"

gearoenix::system::stream::Local::Local(std::fstream file) noexcept
    : file(std::move(file))
{
}

gearoenix::system::stream::Local::Local(const std::string& name, bool writable) noexcept
    : file(name, std::ios::binary | (writable ? std::ios::out : std::ios::in))
{
    if (!file.is_open())
        GXUNEXPECTED
}

gearoenix::system::stream::Local::~Local() noexcept = default;

gearoenix::system::stream::Local* gearoenix::system::stream::Local::open(const std::string& name, const bool writable) noexcept
{
    std::fstream file(name, std::ios::binary | (writable ? std::ios::out : std::ios::in));
    if (!file.is_open())
        return nullptr;
    return new Local(std::move(file));
}

gearoenix::core::Count gearoenix::system::stream::Local::read(void* data, core::Count length) noexcept
{
    file.read((char*)data, length);
    const auto result = (core::Count)file.gcount();
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GXUNEXPECTED
#endif
    file.seekp(file.tellg());
    return result;
}

gearoenix::core::Count gearoenix::system::stream::Local::write(const void* data, core::Count length) noexcept
{
    const core::Count before = (core::Count)file.tellp();
    file.write((const char*)data, length);
    const core::Count result = ((core::Count)file.tellp()) - before;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GXUNEXPECTED
#endif
    file.seekg(file.tellp());
    return result;
}

void gearoenix::system::stream::Local::seek(core::Count offset) noexcept
{
    file.seekg(offset);
    file.seekp(offset);
}

gearoenix::core::Count gearoenix::system::stream::Local::tell() noexcept
{
    return file.tellg();
}

bool gearoenix::system::stream::Local::exist(const std::string& name) noexcept
{
    std::ifstream f(name);
    return f.good();
}

gearoenix::core::Count gearoenix::system::stream::Local::size() noexcept
{
    const auto o = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = file.tellg();
    file.seekg(o);
    return s;
}
