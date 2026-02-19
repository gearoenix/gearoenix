#pragma once
#include "../math/gx-math-numeric.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

namespace gearoenix::core::ecs {
template <std::uint64_t ElementsCount>
struct TypeIndexSet final {
    typedef std::uint64_t element_t;

    constexpr static element_t element_bits_count = sizeof(element_t) << 3;
    constexpr static element_t bits_count = element_bits_count * ElementsCount;
    constexpr static element_t elements_shift = math::Numeric::log2i(element_bits_count);
    constexpr static element_t elements_and_mod = element_bits_count - 1;

private:
    std::array<element_t, ElementsCount> bits = {};

public:
    constexpr TypeIndexSet() = default;

    constexpr TypeIndexSet(const std::initializer_list<element_t> il)
    {
        for (const auto i : il) {
            add(i);
        }
    }

    explicit TypeIndexSet(platform::stream::Stream& s) { read(s); }

    constexpr bool operator==(const TypeIndexSet& other) const = default;
    constexpr bool operator!=(const TypeIndexSet& other) const = default;

    constexpr bool operator<(const TypeIndexSet& other) const
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            if (bits[i] < other.bits[i]) {
                return true;
            }
            if (bits[i] > other.bits[i]) {
                return false;
            }
        }
        return false;
    }

    constexpr bool operator>(const TypeIndexSet& other) const
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            if (bits[i] < other.bits[i]) {
                return false;
            }
            if (bits[i] > other.bits[i]) {
                return true;
            }
        }
        return false;
    }

    constexpr bool operator<=(const TypeIndexSet& other) const
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            if (bits[i] < other.bits[i]) {
                return true;
            }
            if (bits[i] > other.bits[i]) {
                return false;
            }
        }
        return true;
    }

    constexpr bool operator>=(const TypeIndexSet& other) const
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            if (bits[i] < other.bits[i]) {
                return false;
            }
            if (bits[i] > other.bits[i]) {
                return true;
            }
        }
        return true;
    }

    constexpr bool operator[](const element_t index) const
    {
        return contains(index);
    }

    constexpr void add(const element_t type_i)
    {
        bits[type_i >> elements_shift] |= (static_cast<element_t>(1) << (type_i & elements_and_mod));
    }

    constexpr void add(const TypeIndexSet& other)
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            bits[i] |= other.bits[i];
        }
    }

    constexpr void remove(const element_t type_i)
    {
        bits[type_i >> elements_shift] &= ~(static_cast<element_t>(1) << (type_i & elements_and_mod));
    }

    constexpr void remove(const TypeIndexSet& other)
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            bits[i] &= ~other.bits[i];
        }
    }

    [[nodiscard]] constexpr bool contains(const element_t type_i) const
    {
        return type_i < bits_count && static_cast<element_t>(0) != (bits[type_i >> elements_shift] & (static_cast<element_t>(1) << (type_i & elements_and_mod)));
    }

    [[nodiscard]] constexpr bool contains(const TypeIndexSet& o) const
    {
        for (element_t i = 0; i < ElementsCount; ++i) {
            if (o.bits[i] > (bits[i] & o.bits[i])) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr TypeIndexSet common(const TypeIndexSet& o) const
    {
        TypeIndexSet r;
        for (element_t i = 0; i < ElementsCount; ++i) {
            r.bits[i] = bits[i] & o.bits[i];
        }
        return r;
    }

    [[nodiscard]] constexpr TypeIndexSet make_union(const TypeIndexSet& o) const
    {
        TypeIndexSet r;
        for (element_t i = 0; i < ElementsCount; ++i) {
            r.bits[i] = bits[i] | o.bits[i];
        }
        return r;
    }

    [[nodiscard]] constexpr std::vector<element_t> get_type_indices() const
    {
        std::vector<element_t> r;
        for (element_t i = 0; i < bits_count; ++i) {
            if (contains(i)) {
                r.push_back(i);
            }
        }
        return r;
    }

    constexpr void clear()
    {
        for (element_t i = 0; i < bits_count; ++i) {
            bits[i] = 0;
        }
    }

    struct Iterator final {
        const TypeIndexSet* const parent;
        element_t current;

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = element_t;
        using pointer = const element_t*;
        using reference = const element_t&;

        constexpr Iterator(const TypeIndexSet* const parent, const element_t cur)
            : parent(parent)
            , current(cur)
        {
            while (!parent->contains(current) && current < bits_count) {
                ++current;
            }
        }

        constexpr reference operator*() const { return current; }

        constexpr pointer operator->() const { return &current; }

        constexpr Iterator& operator++()
        {
            do {
                ++current;
            } while (current < bits_count && !parent->contains(current));
            return *this;
        }

        constexpr Iterator operator++(int)
        {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        constexpr bool operator==(const Iterator& o) const { return current == o.current; }

        constexpr bool operator!=(const Iterator& o) const { return current != o.current; }
    };

    constexpr Iterator begin() const { return { this, 0 }; }

    constexpr Iterator end() const { return { this, bits_count }; }

    [[nodiscard]] constexpr element_t size() const
    {
        element_t result = 0;
        for (element_t i = 0; i < bits_count; ++i) {
            if (contains(i)) {
                ++result;
            }
        }
        return result;
    }

    void write(platform::stream::Stream& s) const
    {
        for (const auto b : bits) {
            s.write_fail_debug(b);
        }
    }

    void read(platform::stream::Stream& s)
    {
        for (auto& b : bits) {
            s.read(b);
        }
    }
};
}