#ifndef GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#define GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include <string>
#include <type_traits>
#include <vector>

namespace gearoenix::system {
class Application;
}

namespace gearoenix::system::stream {
class Stream {
    GX_GET_VAL_PRT(bool, endian_compatibility, true)
protected:
    Stream() noexcept;
    void built_in_type_read(void* data, core::Count length) noexcept;

public:
    virtual ~Stream() noexcept = default;
    [[nodiscard]] virtual core::Count read(void* data, core::Count length) noexcept = 0;
    [[nodiscard]] virtual core::Count write(const void* data, core::Count length) noexcept = 0;
    virtual void seek(core::Count offset) noexcept = 0;
    [[nodiscard]] virtual core::Count tell() noexcept = 0;
    [[nodiscard]] virtual core::Count size() noexcept = 0;
    [[nodiscard]] std::string read_string() noexcept;
    [[nodiscard]] bool read_bool() noexcept;

    template <typename T>
    typename std::enable_if<sizeof(T) != 1, void>::type
    read(std::vector<T>& data) noexcept
    {
        core::Count c;
        read(c);
        data.resize((size_t)c);
        for (core::Count i = 0; i < c; ++i) {
            read(data[i]);
        }
    }

    template <typename T>
    typename std::enable_if<sizeof(T) == 1, void>::type
    read(std::vector<T>& data) noexcept
    {
        core::Count c;
        read(c);
        data.resize((size_t)c);
        for (core::Count i = 0; i < c; ++i) {
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
    void write(const T& d) noexcept
    {
        write(&d, sizeof(T));
    }
};
}
#endif
