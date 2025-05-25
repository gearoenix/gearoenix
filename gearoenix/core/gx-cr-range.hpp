#pragma once
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>

namespace gearoenix::core {
template <typename T>
struct PtrRange final {
private:
    T* const ptr;
    T* const end_ptr;
    const std::uint32_t count, jump;

public:
    struct Iterator final {
    private:
        T* ptr = nullptr;
        T* end_ptr = nullptr;
        std::uint32_t jump = 0;

    public:
        typedef std::ptrdiff_t difference_type;
        typedef T* value_type;
        typedef value_type& reference;
        typedef value_type* pointer;
        typedef std::forward_iterator_tag iterator_category;

        constexpr Iterator() = default;
        constexpr Iterator(T* ptr, std::uint32_t jump, T* end_ptr);
        ~Iterator() = default;
        constexpr Iterator(const Iterator&);
        constexpr Iterator& operator=(const Iterator&);
        constexpr Iterator& operator++();
        //        constexpr reference operator*() const ;
        constexpr reference operator*();
        template <typename I>
        constexpr Iterator& operator+=(I increment);
        template <typename I>
        constexpr Iterator operator+(I increment) const;
        [[nodiscard]] constexpr bool operator==(const Iterator& o) const;
        [[nodiscard]] constexpr bool operator!=(const Iterator& o) const;
        constexpr void swap(Iterator& o);
        template <typename O>
        [[nodiscard]] constexpr O* cast();
    };

    constexpr PtrRange(T* ptr, std::uint32_t count, std::uint32_t jump);

    [[nodiscard]] constexpr Iterator begin();
    [[nodiscard]] constexpr Iterator end();
};
}

template <typename T>
constexpr gearoenix::core::PtrRange<T>::Iterator::Iterator(T* const ptr, const std::uint32_t jump, T* const end_ptr)
    : ptr(ptr)
    , jump(jump)
    , end_ptr(end_ptr)
{
}

template <typename T>
constexpr gearoenix::core::PtrRange<T>::Iterator::Iterator(const Iterator& o)
    : ptr(o.ptr)
    , jump(o.jump)
    , end_ptr(o.end_ptr)
{
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator& gearoenix::core::PtrRange<T>::Iterator::operator=(const Iterator& o)
{
    if (this == &o)
        return *this;
    ptr = o.ptr;
    jump = o.jump;
    end_ptr = o.end_ptr;
    return *this;
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator& gearoenix::core::PtrRange<T>::Iterator::operator++()
{
    ptr += jump;
    if (end_ptr < ptr)
        ptr = end_ptr;
    return *this;
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator::reference gearoenix::core::PtrRange<T>::Iterator::operator*()
{
    return ptr;
}

template <typename T>
template <typename I>
constexpr typename gearoenix::core::PtrRange<T>::Iterator& gearoenix::core::PtrRange<T>::Iterator::operator+=(const I increment)
{
    ptr += increment * jump;
    if (end_ptr < ptr)
        ptr = end_ptr;
    return *this;
}

template <typename T>
template <typename I>
constexpr typename gearoenix::core::PtrRange<T>::Iterator gearoenix::core::PtrRange<T>::Iterator::operator+(const I increment) const
{
    auto new_ptr = ptr + (increment * jump);
    if (end_ptr < new_ptr)
        new_ptr = end_ptr;
    return Iterator(new_ptr, jump, end_ptr);
}

template <typename T>
constexpr bool gearoenix::core::PtrRange<T>::Iterator::operator==(const Iterator& o) const
{
    return ptr == o.ptr && jump == o.jump && end_ptr == o.end_ptr;
}

template <typename T>
constexpr bool gearoenix::core::PtrRange<T>::Iterator::operator!=(const Iterator& o) const
{
    return ptr != o.ptr || jump != o.jump || end_ptr != o.end_ptr;
}

template <typename T>
constexpr void gearoenix::core::PtrRange<T>::Iterator::swap(Iterator& o)
{
    std::swap(ptr, o.ptr);
    std::swap(jump, o.jump);
    std::swap(end_ptr, o.end_ptr);
}

template <typename T>
template <typename O>
constexpr O* gearoenix::core::PtrRange<T>::Iterator::cast()
{
    return reinterpret_cast<O*>(ptr);
}

template <typename T>
constexpr gearoenix::core::PtrRange<T>::PtrRange(T* const ptr, const std::uint32_t count, const std::uint32_t jump)
    : ptr(ptr)
    , count(count)
    , jump(jump)
    , end_ptr(ptr + count)
{
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator gearoenix::core::PtrRange<T>::begin()
{
    return Iterator(ptr, jump, end_ptr);
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator gearoenix::core::PtrRange<T>::end()
{
    return Iterator(end_ptr, jump, end_ptr);
}