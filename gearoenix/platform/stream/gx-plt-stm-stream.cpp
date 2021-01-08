#include "gx-plt-stm-stream.hpp"

void gearoenix::platform::stream::Stream::built_in_type_read(void* const data, const core::Count length) noexcept
{
    (void)read(data, length);
    if (endian_compatibility)
        return;
    auto* const c_data = static_cast<std::uint8_t*>(data);
    for (core::Count i = 0, j = length - 1; i < j; ++i, --j) {
        const std::uint8_t tmp = c_data[i];
        c_data[i] = c_data[j];
        c_data[j] = tmp;
    }
}

std::string gearoenix::platform::stream::Stream::read_string() noexcept
{
    core::Count c;
    (void)read(c);
    std::string s;
    s.resize(c);
    (void)read(&(s[0]), c);
    return s;
}

bool gearoenix::platform::stream::Stream::read_bool() noexcept
{
    std::uint8_t data;
    (void)read(&data, 1);
    return data != 0;
}

std::vector<std::uint8_t> gearoenix::platform::stream::Stream::get_file_content() noexcept
{
    std::vector<std::uint8_t> result(size());
    seek(0);
    (void)read(result.data(), result.size());
    return result;
}
