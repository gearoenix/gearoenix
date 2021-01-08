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

static std::string create_path(const gearoenix::platform::Application* const app, const std::string& name) noexcept
{
#ifdef GX_PLATFORM_IOS
    (void)app;
    @autoreleasepool {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* path = [paths objectAtIndex:0];
        return gearoenix::core::String::join_path(path, name);
    }
#elif defined(GX_PLATFORM_ANDROID)
    return std::string(app->get_android_application()->activity->internalDataPath) + "/" + name;
#else
    (void)app;
    return name;
#endif
}

gearoenix::platform::stream::Local::Local(const Application* const app, const std::string& name, bool writable) noexcept
    : file(create_path(app, name), std::ios::binary | (writable ? std::ios::out : std::ios::in))
{
    if (!file.is_open() || !file.good())
        GX_LOG_F("Can not open file: " << name)
}

gearoenix::platform::stream::Local::~Local() noexcept = default;

gearoenix::platform::stream::Local* gearoenix::platform::stream::Local::open(const Application* const app, const std::string& name, const bool writable) noexcept
{
    std::fstream file(create_path(app, name), std::ios::binary | (writable ? std::ios::out : std::ios::in));
    if (!file.is_open() || !file.good())
        return nullptr;
    return new Local(std::move(file));
}

gearoenix::core::Count gearoenix::platform::stream::Local::read(void* data, core::Count length) noexcept
{
    file.read((char*)data, static_cast<std::size_t>(length));
    const auto result = (core::Count)file.gcount();
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED
#endif
    file.seekp(file.tellg());
    return result;
}

gearoenix::core::Count gearoenix::platform::stream::Local::write(const void* data, core::Count length) noexcept
{
    const core::Count before = (core::Count)file.tellp();
    file.write((const char*)data, static_cast<std::size_t>(length));
    const core::Count result = ((core::Count)file.tellp()) - before;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED
#endif
    file.seekg(file.tellp());
    return result;
}

void gearoenix::platform::stream::Local::seek(core::Count offset) noexcept
{
    file.seekg(offset);
    file.seekp(offset);
}

gearoenix::core::Count gearoenix::platform::stream::Local::tell() noexcept
{
    return static_cast<core::Count>(file.tellg());
}

bool gearoenix::platform::stream::Local::exist(const Application* const app, const std::string& name) noexcept
{
    std::ifstream f(create_path(app, name));
    return f.is_open() && f.good();
}

gearoenix::core::Count gearoenix::platform::stream::Local::size() noexcept
{
    const auto o = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = file.tellg();
    file.seekg(o);
    return static_cast<core::Count>(s);
}
