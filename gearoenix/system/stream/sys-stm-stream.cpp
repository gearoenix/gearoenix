#include "sys-stm-stream.hpp"
#include "../../core/cr-string.hpp"
#include <fstream>

void gearoenix::system::stream::Stream::built_in_type_read(void* const data, const core::Count length) noexcept
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

std::string gearoenix::system::stream::Stream::read_string() noexcept
{
    core::Count c;
    (void)read(c);
    std::string s;
    s.resize(c);
    (void)read(&(s[0]), c);
    return s;
}

bool gearoenix::system::stream::Stream::read_bool() noexcept
{
    std::uint8_t data;
    (void)read(&data, 1);
    return data != 0;
}

std::vector<std::uint8_t> gearoenix::system::stream::Stream::get_file_content(const std::wstring& address) noexcept
{
    const auto addr = core::String::to_string(address);
    std::ifstream file(addr, std::ios::binary | std::ios::in);
    file.seekg(0, std::ios::end);
    const std::size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<std::uint8_t> file_content(file_size);
    file.read(reinterpret_cast<char*>(file_content.data()), file_size);
    return file_content;
}
