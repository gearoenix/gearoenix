#pragma once
#include "macro/gx-cr-mcr-assert.hpp"
#include <array>

namespace gearoenix::core {
template <typename T, std::size_t Size>
struct GrowingArray final {
private:
    typedef std::array<T, Size> array_t;
    array_t array;
    std::size_t index = 0;

public:
    struct const_iterator final {
    private:
        std::uintptr_t index = 0;
        const GrowingArray& ref;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::forward_iterator_tag;

        constexpr const_iterator(const std::uintptr_t index, const GrowingArray& ref)
            : index(index)
            , ref(ref)
        {
        }

        constexpr const_iterator& operator++()
        {
            if (index < ref.index) {
                ++index;
            } else {
                index = ref.index;
            }
            return *this;
        }

        [[nodiscard]] constexpr reference operator*()
        {
            return ref.array[index];
        }

        [[nodiscard]] constexpr pointer operator->()
        {
            return &ref.array[index];
        }

        [[nodiscard]] constexpr bool operator==(const const_iterator& rhs) const
        {
            return index == rhs.index;
        }

        [[nodiscard]] constexpr bool operator!=(const const_iterator& rhs) const
        {
            return index != rhs.index;
        }
    };

    constexpr void push_back(T&& value)
    {
        GX_ASSERT_D(index < Size);
        array[index++] = std::move(value);
    }

    constexpr void clear()
    {
        for (auto i = 0; i < index; ++i) {
            array[i].~T();
        }
        index = 0;
    }

    template <typename I>
    [[nodiscard]] constexpr const T& operator[](const I i) const
    {
        return array[i];
    }

    [[nodiscard]] constexpr std::size_t size() const
    {
        return index;
    }

    [[nodiscard]] constexpr bool empty() const
    {
        return index == 0;
    }

    [[nodiscard]] constexpr std::uintptr_t max_size()
    {
        return array.size();
    }

    [[nodiscard]] constexpr const_iterator begin() const
    {
        return const_iterator(0, *this);
    }

    [[nodiscard]] constexpr const_iterator end() const
    {
        return const_iterator(index, *this);
    }
};
}
