#include "gx-plt-stm-stream.hpp"
#include "gx-plt-stm-asset.hpp"
#include "gx-plt-stm-local.hpp"
#include "gx-plt-stm-path.hpp"

void gearoenix::platform::stream::Stream::built_in_type_read(void* const data, const std::size_t length) noexcept
{
    (void)read(data, length);
    if (endian_compatibility)
        return;
    auto* const c_data = static_cast<std::uint8_t*>(data);
    for (std::size_t i = 0, j = length - 1; i < j; ++i, --j) {
        const std::uint8_t tmp = c_data[i];
        c_data[i] = c_data[j];
        c_data[j] = tmp;
    }
}

std::shared_ptr<gearoenix::platform::stream::Stream> gearoenix::platform::stream::Stream::open(const Path& path, Application& app) noexcept
{
    if (path.is_asset())
        return std::shared_ptr<Stream>(Asset::construct(app, path.get_raw_data()));
    if (path.is_absolute())
        return std::shared_ptr<Stream>(Local::open(app, path.get_raw_data()));
    return std::shared_ptr<Stream>();
}

void gearoenix::platform::stream::Stream::read(std::string& s) noexcept
{
    std::uint32_t sz;
    read(sz);
    s.resize(sz);
    GX_ASSERT(s.size() == read(s.data(), s.size()));
}

std::vector<std::uint8_t> gearoenix::platform::stream::Stream::get_file_content() noexcept
{
    std::vector<std::uint8_t> result(size());
    seek(0);
    (void)read(result.data(), result.size());
    return result;
}

bool gearoenix::platform::stream::Stream::write(const std::string& s) noexcept
{
    if (sizeof(std::uint32_t) != write(static_cast<std::uint32_t>(s.size())))
        return false;
    if (s.size() != write(s.data(), s.size()))
        return false;
    return true;
}

void gearoenix::platform::stream::Stream::write_fail_debug(const std::string& s) noexcept
{
    GX_ASSERT_D(write(s));
}
