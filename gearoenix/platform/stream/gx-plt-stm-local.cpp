#include "gx-plt-stm-local.hpp"
#include "../gx-plt-application.hpp"

#ifdef GX_PLATFORM_IOS
#include "../../core/gx-cr-string.hpp"
#elif defined(GX_PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#endif

namespace {
std::string create_path(const std::string& name)
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
    return name;
#endif
}

std::ios::openmode create_open_mode(const bool writable)
{
    return std::ios::binary | (writable ? std::ios::out : static_cast<std::ios::openmode>(0)) | std::ios::in;
}

[[nodiscard]] std::fstream create_file(const std::string& name, const bool writable)
{
    std::fstream file(create_path(name), create_open_mode(writable));
    if ((!file.is_open() || !file.good()) && writable) {
        file.open(create_path(name), create_open_mode(writable) | std::ios::trunc);
    }
    return file;
}
}

gearoenix::platform::stream::Local::Local(std::fstream file)
    : file(std::move(file))
{
}

gearoenix::platform::stream::Local::Local(const std::string& name, const bool writable)
    : file(create_file(name, writable))
{
    if (!file.is_open() || !file.good()) {
        GX_LOG_F("Can not open file: " << name);
    }
}

gearoenix::platform::stream::Local::~Local() = default;

gearoenix::platform::stream::Local* gearoenix::platform::stream::Local::open(const std::string& name, const bool writable)
{
    auto file = create_file(name, writable);
    if (!file.is_open() || !file.good()) {
        return nullptr;
    }
    return new Local(std::move(file));
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Local::read(void* const data, const stream_size_t length)
{
    file.read(static_cast<char*>(data), static_cast<std::streamsize>(length));
    const auto result = static_cast<stream_size_t>(file.gcount());
    GX_ASSERT_D(0 != result);
    file.seekp(file.tellg());
    return result;
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Local::write(const void* const data, const stream_size_t length)
{
    const auto before = static_cast<stream_size_t>(file.tellp());
    file.write(static_cast<const char*>(data), static_cast<std::streamsize>(length));
    const auto result = static_cast<stream_size_t>(file.tellp()) - before;
    GX_ASSERT_D(0 != result);
    file.seekg(file.tellp());
    return result;
}

void gearoenix::platform::stream::Local::seek(const stream_size_t offset)
{
    file.seekg(static_cast<std::streamoff>(offset));
    file.seekp(static_cast<std::streamoff>(offset));
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Local::tell()
{
    return static_cast<stream_size_t>(file.tellg());
}

bool gearoenix::platform::stream::Local::exist(const std::string& name)
{
    const std::ifstream f(create_path(name));
    return f.is_open() && f.good();
}

gearoenix::platform::stream::Stream::stream_size_t gearoenix::platform::stream::Local::size()
{
    const auto o = file.tellg();
    file.seekg(0, std::ios::end);
    const auto s = file.tellg();
    file.seekg(o);
    return static_cast<stream_size_t>(s);
}

void gearoenix::platform::stream::Local::flush()
{
    file.flush();
}
