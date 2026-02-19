#include "gx-plt-stm-stream.hpp"
#include "gx-plt-stm-asset.hpp"
#include "gx-plt-stm-local.hpp"
#include "gx-plt-stm-path.hpp"

void gearoenix::platform::stream::Stream::built_in_type_read(void* const data, const stream_size_t length)
{
    (void)read(data, length);
    if (endian_compatibility) {
        return;
    }
    auto* const c_data = static_cast<std::uint8_t*>(data);
    for (stream_size_t i = 0, j = length - 1; i < j; ++i, --j) {
        const std::uint8_t tmp = c_data[i];
        c_data[i] = c_data[j];
        c_data[j] = tmp;
    }
}

std::shared_ptr<gearoenix::platform::stream::Stream> gearoenix::platform::stream::Stream::open(const Path& path)
{
    if (path.is_asset()) {
        return std::shared_ptr<Stream>(Asset::construct(path.get_raw_data()));
    }
    if (path.is_absolute()) {
        return std::shared_ptr<Stream>(Local::open(path.get_raw_data(), true));
    }
    return {};
}

void gearoenix::platform::stream::Stream::read(std::string& s)
{
    stream_size_t sz = 0;
    read(sz);
    s.resize(sz);
    GX_ASSERT(s.size() == read(s.data(), s.size()));
}

void gearoenix::platform::stream::Stream::read(Stream& s)
{
    const auto p = s.tell();
    std::vector<std::uint8_t> data;
    read(data);
    GX_ASSERT(data.size() == s.write(data.data(), data.size()));
    s.seek(p);
}

std::vector<std::uint8_t> gearoenix::platform::stream::Stream::get_file_content()
{
    std::vector<std::uint8_t> result(size());
    seek(0);
    (void)read(result.data(), result.size());
    return result;
}

void gearoenix::platform::stream::Stream::write(Stream& s) { write(s.get_file_content()); }

bool gearoenix::platform::stream::Stream::write(const std::string& s)
{
    if (sizeof(stream_size_t) != write(static_cast<stream_size_t>(s.size()))) {
        return false;
    }
    if (s.size() != write(s.data(), s.size())) {
        return false;
    }
    return true;
}

void gearoenix::platform::stream::Stream::write_fail_debug(const std::string& s)
{
    [[maybe_unused]] const auto b = write(s);
    GX_ASSERT_D(b);
}
