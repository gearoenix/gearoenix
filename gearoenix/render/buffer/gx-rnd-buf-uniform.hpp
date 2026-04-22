#pragma once
#include "../../core/macro/gx-cr-mcr-assert.hpp"

#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::render::buffer {
struct Uniform final {
    using cns_args_t = std::pair<std::shared_ptr<core::allocator::Range>, std::uint32_t>;

private:
    static std::size_t frame_pointer;

public:
    const std::shared_ptr<core::allocator::Range> range;
    const std::size_t size;
    const std::size_t offset_from_frame_pointer;
    const std::uint32_t shader_data_index;

    explicit Uniform(cns_args_t&& args);
    Uniform(const Uniform&) = default;
    ~Uniform();

    static void set_frame_pointer(const std::size_t ptr)
    {
        frame_pointer = ptr;
    }

    [[nodiscard]] static std::size_t get_frame_pointer()
    {
        return frame_pointer;
    }

    void upload(const void* const data, const std::size_t sz)
    {
        GX_ASSERT_D(frame_pointer > 0);
        const auto dst_off = frame_pointer + offset_from_frame_pointer;
        GX_ASSERT_D(sz <= size);
        std::memcpy(reinterpret_cast<void*>(dst_off), data, sz);
    }

    template <typename T>
    T& get_ref()
    {
        GX_ASSERT_D(frame_pointer > 0);
        const auto dst_off = frame_pointer + offset_from_frame_pointer;
        GX_ASSERT_D(sizeof(T) <= size);
        return *reinterpret_cast<T*>(dst_off);
    }

    template <typename T>
    const T& get_ref() const
    {
        GX_ASSERT_D(frame_pointer > 0);
        const auto dst_off = frame_pointer + offset_from_frame_pointer;
        GX_ASSERT_D(sizeof(T) <= size);
        return *reinterpret_cast<T*>(dst_off);
    }

    template <typename T>
    void upload(const T& data)
    {
        upload(&data, sizeof(T));
    }

    template <typename T>
    void upload(const std::vector<T>& data)
    {
        upload(data.data(), sizeof(T) * data.size());
    }
};
}