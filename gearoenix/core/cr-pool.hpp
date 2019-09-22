#ifndef GEAROENIX_CORE_POOL_HPP
#define GEAROENIX_CORE_POOL_HPP

#include "../system/sys-log.hpp"
#include "cr-build-configuration.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace gearoenix::core {
template <class T>
class OneLoopPool {
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

        iterator(std::size_t index, OneLoopPool* pool) noexcept;

        iterator& operator++() noexcept;
        reference operator*() noexcept;
        pointer operator->() noexcept;
        bool operator==(const iterator& rhs) const noexcept;
        bool operator!=(const iterator& rhs) const noexcept;

    private:
        std::size_t index = 0;
        OneLoopPool* pool = nullptr;
    };

    ~OneLoopPool() noexcept;
    T* get_next(std::function<T*()>) noexcept;
    void refresh() noexcept;
    void set_increase_rate(std::size_t) noexcept;
    iterator begin() noexcept;
    iterator end() noexcept;
    std::size_t capacity() const noexcept;
    std::size_t size() const noexcept;
    template <typename I>
    typename std::enable_if<std::is_integral<I>::value, const T&>::type operator[](I index) const noexcept;
    template <typename I>
    typename std::enable_if<std::is_integral<I>::value, T&>::type operator[](I index) noexcept;
};
}

template <class T>
typename gearoenix::core::OneLoopPool<T>::iterator& gearoenix::core::OneLoopPool<T>::iterator::operator++() noexcept
{
    if (index < pool->current_index)
        ++index;
    return *this;
}

template <class T>
typename gearoenix::core::OneLoopPool<T>::iterator::reference gearoenix::core::OneLoopPool<T>::iterator::operator*() noexcept
{
    return *(pool->objects[index]);
}

template <class T>
typename gearoenix::core::OneLoopPool<T>::iterator::pointer gearoenix::core::OneLoopPool<T>::iterator::operator->() noexcept
{
    return pool->objects[index];
}

template <class T>
bool gearoenix::core::OneLoopPool<T>::iterator::operator==(const gearoenix::core::OneLoopPool<T>::iterator& rhs) const noexcept
{
    return index == rhs.index;
}

template <class T>
bool gearoenix::core::OneLoopPool<T>::iterator::operator!=(const gearoenix::core::OneLoopPool<T>::iterator& rhs) const noexcept
{
    return index != rhs.index;
}

template <class T>
gearoenix::core::OneLoopPool<T>::iterator::iterator(std::size_t index, OneLoopPool<T>* pool) noexcept
    : index(index)
    , pool(pool)
{
}

template <class T>
gearoenix::core::OneLoopPool<T>::~OneLoopPool() noexcept
{
	objects.clear();
}

template <class T>
T* gearoenix::core::OneLoopPool<T>::get_next(std::function<T*()> f) noexcept
{
    if (current_index < objects.size())
        return objects[current_index++].get();
    for (std::size_t i = 0; i < increase_rate; ++i)
        objects.push_back(std::unique_ptr<T>(f()));
    return objects[current_index++].get();
}

template <class T>
void gearoenix::core::OneLoopPool<T>::refresh() noexcept
{
    current_index = 0;
}

template <class T>
void gearoenix::core::OneLoopPool<T>::set_increase_rate(const std::size_t i) noexcept
{
    increase_rate = i;
}

template <class T>
typename gearoenix::core::OneLoopPool<T>::iterator gearoenix::core::OneLoopPool<T>::begin() noexcept
{
    return iterator(0, this);
}

template <class T>
typename gearoenix::core::OneLoopPool<T>::iterator gearoenix::core::OneLoopPool<T>::end() noexcept
{
    return iterator(current_index, this);
}

template <class T>
std::size_t gearoenix::core::OneLoopPool<T>::capacity() const noexcept
{
    return objects.size();
}

template <class T>
std::size_t gearoenix::core::OneLoopPool<T>::size() const noexcept
{
    return current_index;
}

template <class T>
template <typename I>
typename std::enable_if<std::is_integral<I>::value, const T&>::type
    gearoenix::core::OneLoopPool<T>::operator[](const I index) const noexcept
{
#ifdef GX_DEBUG_MODE
    if (index < 0 || static_cast<std::size_t>(index) >= current_index)
        GXLOGF("Out of range index {" << index << "}");
#endif
    return *(objects[index].get());
}

template <class T>
template <typename I>
typename std::enable_if<std::is_integral<I>::value, T&>::type
    gearoenix::core::OneLoopPool<T>::operator[](const I index) noexcept
{
#ifdef GX_DEBUG_MODE
    if (index < 0 || static_cast<std::size_t>(index) >= current_index)
        GXLOGF("Out of range index {" << index << "}");
#endif
    return *(objects[index].get());
}

#endif