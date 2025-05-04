#pragma once
#include "macro/gx-cr-mcr-assert.hpp"
#include <array>

namespace gearoenix::core {
template <typename T, std::size_t Size>
struct GrowingArray final : std::array<T, Size> {
private:
    std::size_t index = 0;

public:
    constexpr void push_back(T&& value)
    {
        GX_ASSERT_D(index < Size);
        this->operator[](index++) = std::move(value);
    }

    constexpr void clear()
    {
        for (auto i = 0; i < index; ++i) {
            this->operator[](i).~T();
        }
        index = 0;
    }

    [[nodiscard]] constexpr std::size_t size() const
    {
        return index;
    }
};
}
