#pragma once
#include "../../core/macro/gx-cr-mcr-assert.hpp"

#include <cstddef>
#include <cstring>
#include <vector>

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::render::buffer {
struct Uniform final {
private:
    static std::size_t frame_pointer;

public:
    const std::shared_ptr<core::allocator::Range> range;
    const std::size_t size;
    const std::size_t offset_from_frame_pointer;

    explicit Uniform(std::shared_ptr<core::allocator::Range>&& range);
    Uniform(const Uniform&);
    ~Uniform();

    static void set_frame_pointer(const std::size_t ptr)
    {
        frame_pointer = ptr;
    }

    void upload(const void * const data, const std::size_t sz)
    {
        const auto dst_off = frame_pointer + offset_from_frame_pointer;
        GX_ASSERT_D(sz <= size);
        std::memcpy(reinterpret_cast<void*>(dst_off), data, sz);
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