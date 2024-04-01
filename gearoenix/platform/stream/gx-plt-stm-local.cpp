#include "gx-plt-stm-local.hpp"
#include "../gx-plt-application.hpp"

#ifdef GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#endif

namespace {
std::string create_path(const gearoenix::platform::Application& app, const std::string& name)
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

std::ios::openmode create_open_mode(const bool writable)
{
    return std::ios::binary | (writable ? std::ios::out : static_cast<std::ios::openmode>(0)) | std::ios::in;
}
}

gearoenix::platform::stream::Local::Local(std::fstream file)
    : file(std::move(file))
{
}

gearoenix::platform::stream::Local::Local(const Application& app, const std::string& name, bool writable)
    : file(create_path(app, name), create_open_mode(writable))
{
    if ((!file.is_open() || !file.good()) && writable) {
        file.open(create_path(app, name), create_open_mode(writable) | std::ios::trunc);
    }
    if (!file.is_open() || !file.good()) {
        GX_LOG_F("Can not open file: " << name);
    }
}

gearoenix::platform::stream::Local::~Local() = default;

gearoenix::platform::stream::Local* gearoenix::platform::stream::Local::open(const Application& app, const std::string& name, const bool writable)
{
    std::fstream file(create_path(app, name), std::ios::binary | (writable ? std::ios::out : static_cast<std::ios::openmode>(0)) | std::ios::in);
    if (!file.is_open() || !file.good())
        return nullptr;
    return new Local(std::move(file));
}

std::size_t gearoenix::platform::stream::Local::read(void* const data, const std::size_t length)
{
    file.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(length));
    const auto result = (std::size_t)file.gcount();
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    file.seekp(file.tellg());
    return result;
}

std::size_t gearoenix::platform::stream::Local::write(const void* const data, const std::size_t length)
{
    const std::size_t before = (std::size_t)file.tellp();
    file.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(length));
    const std::size_t result = ((std::size_t)file.tellp()) - before;
#ifdef GX_DEBUG_MODE
    if (0 == result)
        GX_UNEXPECTED;
#endif
    file.seekg(file.tellp());
    return result;
}

void gearoenix::platform::stream::Local::seek(std::size_t offset)
{
    file.seekg(static_cast<std::streamoff>(offset));
    file.seekp(static_cast<std::streamoff>(offset));
}

std::size_t gearoenix::platform::stream::Local::tell()
{
    return static_cast<std::size_t>(file.tellg());
}

bool gearoenix::platform::stream::Local::exist(const Application& app, const std::string& name)
{
    const std::ifstream f(create_path(app, name));
    return f.is_open() && f.good();
}

std::size_t gearoenix::platform::stream::Local::size()
{
    const auto o = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = file.tellg();
    file.seekg(o);
    return static_cast<std::size_t>(s);
}

void gearoenix::platform::stream::Local::flush()
{
    file.flush();
}
