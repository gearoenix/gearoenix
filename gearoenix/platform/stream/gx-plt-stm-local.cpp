#include "gx-plt-stm-local.hpp"
#include "../../core/gx-cr-build-configuration.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-log.hpp"

#ifdef GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#endif

gearoenix::platform::stream::Local::Local(std::fstream file) noexcept
    : file(std::move(file))
{
}

static std::string create_path(const gearoenix::platform::Application& app, const std::string& name) noexcept
{
#ifdef GX_PLATFORM_IOS
    (void)app;
    @autoreleasepool {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* path = [paths objectAtIndex:0];
        return gearoenix::core::String::join_path(path, name);
    }
#elif defined(GX_PLATFORM_ANDROID)
    return std::string(app.get_android_application()->activity->internalDataPath) + "/" + name;
#else
    (void)app;
    return name;
#endif
}

gearoenix::platform::stream::Local::Local(const Application& app, const std::string& name, bool writable) noexcept
    : file(create_path(app, name), std::ios::binary | (writable ? std::ios::out : std::ios::in))
{
    if (!file.is_open() || !file.good())
        GX_LOG_F("Can not open file: " << name);
}

gearoenix::platform::stream::Local::~Local() noexcept = default;

gearoenix::platform::stream::Local* gearoenix::platform::stream::Local::open(const Application& app, const std::string& name, const bool writable) noexcept
{
    std::fstream file(create_path(app, name), std::ios::binary | (writable ? std::ios::out : std::ios::in));
    if (!file.is_open() || !file.good())
        return nullptr;
    return new Local(std::move(file));
}

std::size_t gearoenix::platform::stream::Local::read(void* data, std::size_t length) noexcept
{
    file.read((char*)data, static_cast<std::size_t>(length));
    const auto result = (std::size_t)file.gcount();
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    file.seekp(file.tellg());
    return result;
}

std::size_t gearoenix::platform::stream::Local::write(const void* data, std::size_t length) noexcept
{
    const std::size_t before = (std::size_t)file.tellp();
    file.write((const char*)data, static_cast<std::size_t>(length));
    const std::size_t result = ((std::size_t)file.tellp()) - before;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    file.seekg(file.tellp());
    return result;
}

void gearoenix::platform::stream::Local::seek(std::size_t offset) noexcept
{
    file.seekg(offset);
    file.seekp(offset);
}

std::size_t gearoenix::platform::stream::Local::tell() noexcept
{
    return static_cast<std::size_t>(file.tellg());
}

bool gearoenix::platform::stream::Local::exist(const Application& app, const std::string& name) noexcept
{
    std::ifstream f(create_path(app, name));
    return f.is_open() && f.good();
}

std::size_t gearoenix::platform::stream::Local::size() noexcept
{
    const auto o = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = file.tellg();
    file.seekg(o);
    return static_cast<std::size_t>(s);
}
