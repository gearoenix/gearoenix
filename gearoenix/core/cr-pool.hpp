#ifndef GEAROENIX_CORE_POOL_HPP
#define GEAROENIX_CORE_POOL_HPP

#include "../system/sys-log.hpp"
#include "cr-build-configuration.hpp"
#include <functional>
#include <memory>
#include <vector>

#ifdef GX_DEBUG_MODE
#define GX_CORE_POOL_INDEX_FUNC_EXPR
#else
#define GX_CORE_POOL_INDEX_FUNC_EXPR constexpr
#endif

namespace gearoenix::core {
template <typename T>
struct OneLoopPool {
private:
    std::vector<std::unique_ptr<T>> objects;
    std::size_t current_index = 0;
    std::size_t increase_rate = 1;

public:
    struct iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr iterator(std::size_t index, OneLoopPool* pool) noexcept;

        constexpr iterator& operator++() noexcept;
        [[nodiscard]] constexpr reference operator*() noexcept;
        [[nodiscard]] constexpr pointer operator->() noexcept;
        [[nodiscard]] constexpr bool operator==(const iterator& rhs) const noexcept;
        [[nodiscard]] constexpr bool operator!=(const iterator& rhs) const noexcept;

    private:
        std::size_t index = 0;
        OneLoopPool* const pool;
    };

    struct const_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr const_iterator(std::size_t index, const OneLoopPool* pool) noexcept;

        constexpr const_iterator& operator++() noexcept;
        [[nodiscard]] constexpr reference operator*() noexcept;
        [[nodiscard]] constexpr pointer operator->() noexcept;
        [[nodiscard]] constexpr bool operator==(const const_iterator& rhs) const noexcept;
        [[nodiscard]] constexpr bool operator!=(const const_iterator& rhs) const noexcept;

    private:
        std::size_t index = 0;
        const OneLoopPool* const pool = nullptr;
    };
    template <typename F>
    [[nodiscard]] constexpr T* get_next(const F&& func) noexcept;
    constexpr void refresh() noexcept;
    constexpr void set_increase_rate(std::size_t) noexcept;
    [[nodiscard]] constexpr iterator begin() noexcept;
    [[nodiscard]] constexpr iterator end() noexcept;
    [[nodiscard]] constexpr const_iterator begin() const noexcept;
    [[nodiscard]] constexpr const_iterator end() const noexcept;
    [[nodiscard]] constexpr std::size_t capacity() const noexcept;
    [[nodiscard]] constexpr std::size_t size() const noexcept;
    template <typename I>
    [[nodiscard]] GX_CORE_POOL_INDEX_FUNC_EXPR const T& operator[](I index) const noexcept;
    template <typename I>
    [[nodiscard]] GX_CORE_POOL_INDEX_FUNC_EXPR T& operator[](I index) noexcept;
};
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::iterator& gearoenix::core::OneLoopPool<T>::iterator::operator++() noexcept
{
    if (index < pool->current_index)
        ++index;
    return *this;
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::iterator::reference gearoenix::core::OneLoopPool<T>::iterator::operator*() noexcept
{
    return *(pool->objects[index]);
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::iterator::pointer gearoenix::core::OneLoopPool<T>::iterator::operator->() noexcept
{
    return pool->objects[index];
}

template <class T>
constexpr bool gearoenix::core::OneLoopPool<T>::iterator::operator==(const gearoenix::core::OneLoopPool<T>::iterator& rhs) const noexcept
{
    return index == rhs.index;
}

template <class T>
constexpr bool gearoenix::core::OneLoopPool<T>::iterator::operator!=(const gearoenix::core::OneLoopPool<T>::iterator& rhs) const noexcept
{
    return index != rhs.index;
}

template <class T>
constexpr gearoenix::core::OneLoopPool<T>::iterator::iterator(const std::size_t index, OneLoopPool<T>* const pool) noexcept
    : index(index)
    , pool(pool)
{
}

template <class T>
constexpr gearoenix::core::OneLoopPool<T>::const_iterator::const_iterator(const std::size_t index, const OneLoopPool* const pool) noexcept
    : index(index)
    , pool(pool)
{
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::const_iterator& gearoenix::core::OneLoopPool<T>::const_iterator::operator++() noexcept
{
    if (index < pool->current_index)
        ++index;
    return *this;
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::const_iterator::reference gearoenix::core::OneLoopPool<T>::const_iterator::operator*() noexcept
{
    return *(pool->objects[index]);
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::const_iterator::pointer gearoenix::core::OneLoopPool<T>::const_iterator::operator->() noexcept
{
    return pool->objects[index];
}

template <class T>
constexpr bool gearoenix::core::OneLoopPool<T>::const_iterator::operator==(const gearoenix::core::OneLoopPool<T>::const_iterator& rhs) const noexcept
{
    return index == rhs.index;
}

template <class T>
constexpr bool gearoenix::core::OneLoopPool<T>::const_iterator::operator!=(const gearoenix::core::OneLoopPool<T>::const_iterator& rhs) const noexcept
{
    return index != rhs.index;
}

template <class T>
template <typename F>
constexpr T* gearoenix::core::OneLoopPool<T>::get_next(const F&& f) noexcept
{
    if (current_index < objects.size())
        return objects[current_index++].get();
    for (std::size_t i = 0; i < increase_rate; ++i)
        objects.emplace_back(f());
    return objects[current_index++].get();
}

template <class T>
constexpr void gearoenix::core::OneLoopPool<T>::refresh() noexcept
{
    current_index = 0;
}

template <class T>
constexpr void gearoenix::core::OneLoopPool<T>::set_increase_rate(const std::size_t i) noexcept
{
    increase_rate = i;
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::iterator gearoenix::core::OneLoopPool<T>::begin() noexcept
{
    return iterator(0, this);
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::iterator gearoenix::core::OneLoopPool<T>::end() noexcept
{
    return iterator(current_index, this);
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::const_iterator gearoenix::core::OneLoopPool<T>::begin() const noexcept
{
    return const_iterator(0, this);
}

template <class T>
constexpr typename gearoenix::core::OneLoopPool<T>::const_iterator gearoenix::core::OneLoopPool<T>::end() const noexcept
{
    return const_iterator(current_index, this);
}

template <class T>
constexpr std::size_t gearoenix::core::OneLoopPool<T>::capacity() const noexcept
{
    return objects.size();
}

template <class T>
constexpr std::size_t gearoenix::core::OneLoopPool<T>::size() const noexcept
{
    return current_index;
}

template <class T>
template <typename I>
GX_CORE_POOL_INDEX_FUNC_EXPR const T& gearoenix::core::OneLoopPool<T>::operator[](const I index) const noexcept
{
    static_assert(std::is_integral_v<I>, "Index must have integer type.");
#ifdef GX_DEBUG_MODE
    if (index < 0 || static_cast<std::size_t>(index) >= current_index)
        GXLOGF("Out of range index {" << index << "}")
#endif
    return *(objects[index].get());
}

template <class T>
template <typename I>
GX_CORE_POOL_INDEX_FUNC_EXPR T& gearoenix::core::OneLoopPool<T>::operator[](const I index) noexcept
{
    static_assert(std::is_integral_v<I>, "Index must have integer type.");
#ifdef GX_DEBUG_MODE
    if (index < 0 || static_cast<std::size_t>(index) >= current_index)
        GXLOGF("Out of range index {" << index << "}")
#endif
    return *(objects[index].get());
}

#endif