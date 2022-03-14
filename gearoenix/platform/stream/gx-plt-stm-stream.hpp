#ifndef GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#define GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <string>
#include <type_traits>
#include <vector>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Stream {
    GX_GETSET_VAL_PRT(bool, endian_compatibility, true)

protected:
    Stream() noexcept = default;
    void built_in_type_read(void* data, std::size_t length) noexcept;

public:
    virtual ~Stream() noexcept = default;
    [[nodiscard]] virtual std::size_t read(void* data, std::size_t length) noexcept = 0;
    [[nodiscard]] virtual std::size_t write(const void* data, std::size_t length) noexcept = 0;
    virtual void seek(std::size_t offset) noexcept = 0;
    [[nodiscard]] virtual std::size_t tell() noexcept = 0;
    [[nodiscard]] virtual std::size_t size() noexcept = 0;
    [[nodiscard]] std::string read_string() noexcept;
    [[nodiscard]] bool read_bool() noexcept;

    template <typename T>
    typename std::enable_if<sizeof(T) != 1, void>::type
    read(std::vector<T>& data) noexcept
    {
        std::size_t c;
        read(c);
        data.resize((size_t)c);
        for (std::size_t i = 0; i < c; ++i) {
            read(data[i]);
        }
    }

    template <typename T>
    typename std::enable_if<sizeof(T) == 1, void>::type
    read(std::vector<T>& data) noexcept
    {
        std::size_t c;
        read(c);
        data.resize((size_t)c);
        for (std::size_t i = 0; i < c; ++i) {
            read(data[i]);
        }
    }

    template <typename T>
    typename std::enable_if<
        sizeof(T) == 1 && (std::is_integral<T>::value || std::is_enum<T>::value), void>::type
    read(T& data) noexcept
    {
        (void)read(&data, sizeof(T));
    }

    template <typename T>
    typename std::enable_if<(sizeof(T) > 1) && (sizeof(T) < 9) && (std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_enum<T>::value), void>::type
    read(T& data) noexcept
    {
        built_in_type_read(&data, sizeof(T));
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

    [[nodiscard]] std::vector<std::uint8_t> get_file_content() noexcept;
};
}
#endif
