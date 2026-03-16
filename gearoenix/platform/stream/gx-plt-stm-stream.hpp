#pragma once
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
    typedef std::uint64_t stream_size_t;
    GX_GETSET_VAL_PRT(bool, endian_compatibility, true);

    Stream() = default;
    void built_in_type_read(void* data, stream_size_t length);

public:
    virtual ~Stream() = default;
    [[nodiscard]] static std::shared_ptr<Stream> open(const Path& path);
    [[nodiscard]] virtual stream_size_t read(void* data, stream_size_t length) = 0;
    [[nodiscard]] virtual stream_size_t write(const void* data, stream_size_t length) = 0;
    virtual void seek(stream_size_t offset) = 0;
    [[nodiscard]] virtual stream_size_t tell() = 0;
    [[nodiscard]] virtual stream_size_t size() = 0;
    virtual void flush() = 0;
    [[nodiscard]] virtual std::vector<std::uint8_t> get_file_content();

    void read(std::string& s);

    /// \note Remember to seek to zero if you are calling this function with a newly built stream.
    void read(Stream& s);

    template <typename T>
    void read(std::vector<T>& data)
    {
        const auto c = read<stream_size_t>();
        data.resize(static_cast<std::uintptr_t>(c));
        if constexpr (sizeof(T) == 1) {
            GX_ASSERT(data.size() == read(data.data(), data.size()));
        } else {
            for (auto i = decltype(c) { 0 }; i < c; ++i) {
                read(data[i]);
            }
        }
    }

    template <typename T>
    void read(T& data)
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
    [[nodiscard]] T read()
    {
        T data;
        read(data);
        return data;
    }

    void write(Stream& s);

    template <typename T>
    [[nodiscard]] stream_size_t write(const T& d) { return write(&d, sizeof(T)); }

    template <typename T>
    void write(const std::vector<T>& data)
    {
        const auto c = static_cast<stream_size_t>(data.size());
        GX_ASSERT(sizeof(c) == write(c));
        if constexpr (sizeof(T) == 1) {
            GX_ASSERT(data.size() == write(data.data(), data.size()));
        } else {
            for (auto i = decltype(c) { 0 }; i < c; ++i) {
                GX_ASSERT(sizeof(T) == write(data[i]));
            }
        }
    }

    [[nodiscard]] bool write(const std::string& s);

    template <typename T>
    void write_fail_debug(const T& d)
    {
        [[maybe_unused]] const auto sz = write(&d, sizeof(T));
        GX_ASSERT_D(sz == sizeof(T));
    }

    void write_fail_debug(const std::string& s);
};
}