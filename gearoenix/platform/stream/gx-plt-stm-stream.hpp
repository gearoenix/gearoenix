#ifndef GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#define GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Path;
struct Stream {
    GX_GETSET_VAL_PRT(bool, endian_compatibility, true)

protected:
    Stream() noexcept = default;
    void built_in_type_read(void* data, std::size_t length) noexcept;

public:
    virtual ~Stream() noexcept = default;
    [[nodiscard]] static std::shared_ptr<Stream> open(const Path& path, Application& app) noexcept;
    [[nodiscard]] virtual std::size_t read(void* data, std::size_t length) noexcept = 0;
    [[nodiscard]] virtual std::size_t write(const void* data, std::size_t length) noexcept = 0;
    virtual void seek(std::size_t offset) noexcept = 0;
    [[nodiscard]] virtual std::size_t tell() noexcept = 0;
    [[nodiscard]] virtual std::size_t size() noexcept = 0;

    void read(std::string& s) noexcept;

    template <typename T>
    void read(std::vector<T>& data) noexcept
    {
        const auto c = read<std::uint32_t>();
        data.resize(static_cast<std::size_t>(c));
        if constexpr (sizeof(T) == 1) {
            GX_ASSERT(data.size() == read(data.data(), data.size()));
        } else {
            for (std::uint32_t i = 0; i < c; ++i) {
                read(data[i]);
            }
        }
    }

    template <typename T>
    void read(T& data) noexcept
    {
        constexpr auto b1 = sizeof(T) == 1;
        constexpr auto b2 = sizeof(T) < 9 && (std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>);
        static_assert(b1 || b2, "Unexpected type to read.");
        if constexpr (b1) {
            GX_ASSERT(1 == read(&data, 1));
        } else {
            built_in_type_read(&data, sizeof(T));
        }
    }

    template <typename T>
    [[nodiscard]] T read() noexcept
    {
        T data;
        read(data);
        return data;
    }

    template <typename T>
    [[nodiscard]] std::size_t write(const T& d) noexcept
    {
        return write(&d, sizeof(T));
    }

    [[nodiscard]] bool write(const std::string& s) noexcept;

    template <typename T>
    void write_fail_debug(const T& d) noexcept
    {
        [[maybe_unused]] const auto sz = write(&d, sizeof(T));
        GX_ASSERT_D(sz == sizeof(T));
    }

    void write_fail_debug(const std::string& s) noexcept;

    [[nodiscard]] std::vector<std::uint8_t> get_file_content() noexcept;
};
}
#endif
