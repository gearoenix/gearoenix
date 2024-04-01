#ifndef GEAROENIX_CORE_POOL_HPP
#define GEAROENIX_CORE_POOL_HPP
#include "../platform/gx-plt-log.hpp"
#include "gx-cr-build-configuration.hpp"
#include <functional>
#include <vector>

namespace gearoenix::core {
template <typename T>
struct Pool final {
private:
    std::vector<T> objects;
    std::size_t size = 0;

public:
    struct iterator final {
        using difference_type = typename std::vector<T>::iterator::difference_type;
        using value_type = typename std::vector<T>::iterator::value_type;
        using pointer = typename std::vector<T>::iterator::pointer;
        using reference = typename std::vector<T>::iterator::reference;
        using iterator_category = typename std::vector<T>::iterator::iterator_category;

        constexpr iterator(const std::size_t index, Pool* const pool)
            : index(index)
            , pool(pool)
        {
        }

        constexpr iterator& operator++()
        {
            if (index < pool->size)
                ++index;
            else
                index = pool->size;
            return *this;
        }

        [[nodiscard]] constexpr reference operator*()
        {
            return pool->objects[index];
        }

        [[nodiscard]] constexpr pointer operator->()
        {
            return &pool->objects[index];
        }

        [[nodiscard]] constexpr bool operator==(const iterator& rhs) const
        {
            return index == rhs.index;
        }

        [[nodiscard]] constexpr bool operator!=(const iterator& rhs) const
        {
            return index != rhs.index;
        }

    private:
        std::size_t index = 0;
        Pool* const pool = nullptr;
    };

    struct const_iterator final {
        using difference_type = typename std::vector<T>::const_iterator::difference_type;
        using value_type = typename std::vector<T>::const_iterator::value_type;
        using pointer = typename std::vector<T>::const_iterator::pointer;
        using reference = typename std::vector<T>::const_iterator::reference;
        using iterator_category = typename std::vector<T>::const_iterator::iterator_category;

        constexpr const_iterator(const std::size_t index, const Pool* const pool)
            : index(index)
            , pool(pool)
        {
        }

        constexpr const_iterator& operator++()
        {
            if (index < pool->size)
                ++index;
            else
                index = pool->size;
            return *this;
        }

        [[nodiscard]] constexpr reference operator*()
        {
            return pool->objects[index];
        }

        [[nodiscard]] constexpr pointer operator->()
        {
            return &pool->objects[index];
        }

        [[nodiscard]] constexpr bool operator==(const const_iterator& rhs) const
        {
            return index == rhs.index;
        }

        [[nodiscard]] constexpr bool operator!=(const const_iterator& rhs) const
        {
            return index != rhs.index;
        }

    private:
        std::size_t index = 0;
        const Pool* const pool = nullptr;
    };

    template <typename F>
    [[nodiscard]] constexpr std::size_t emplace(const F&& func)
    {
        for (std::size_t i = objects.size(); i <= size; ++i)
            objects.emplace_back(func());
        return size++;
    }

    constexpr void clear()
    {
        size = 0;
    }

    [[nodiscard]] constexpr std::size_t get_size()
    {
        return size;
    }

    [[nodiscard]] constexpr iterator begin()
    {
        return iterator(0, this);
    }

    [[nodiscard]] constexpr iterator end()
    {
        return iterator(size, this);
    }

    [[nodiscard]] constexpr const_iterator begin() const
    {
        return const_iterator(0, this);
    }

    [[nodiscard]] constexpr const_iterator end() const
    {
        return const_iterator(size, this);
    }

    template <typename I>
    [[nodiscard]] constexpr const T& operator[](const I index) const
    {
        return objects[static_cast<std::size_t>(index)];
    }

    template <typename I>
    [[nodiscard]] constexpr T& operator[](const I index)
    {
        return objects[static_cast<std::size_t>(index)];
    }
};
}
#endif
