#ifndef GEAROENIX_CORE_RANGE_HPP
#define GEAROENIX_CORE_RANGE_HPP

#include <cstddef>
#include <iterator>
#include <utility>

namespace gearoenix::core {
template <typename T>
struct PtrRange final {
private:
    T* const ptr;
    T* const end_ptr;
    const std::size_t count, jump;

public:
    struct Iterator final {
    private:
        T* ptr = nullptr;
        T* end_ptr = nullptr;
        std::size_t jump = 0;

    public:
        typedef std::ptrdiff_t difference_type;
        typedef T* value_type;
        typedef value_type& reference;
        typedef value_type* pointer;
        typedef std::forward_iterator_tag iterator_category;

        constexpr Iterator() noexcept = default;
        constexpr Iterator(T* ptr, std::size_t jump, T* end_ptr) noexcept;
        ~Iterator() noexcept = default;
        constexpr Iterator(const Iterator&) noexcept;
        constexpr Iterator& operator=(const Iterator&) noexcept;
        constexpr Iterator& operator++() noexcept;
        //        constexpr reference operator*() const noexcept;
        constexpr reference operator*() noexcept;
        template <typename I>
        constexpr Iterator& operator+=(I increment) noexcept;
        template <typename I>
        constexpr Iterator operator+(I increment) const noexcept;
        [[nodiscard]] constexpr bool operator==(const Iterator& o) const noexcept;
        [[nodiscard]] constexpr bool operator!=(const Iterator& o) const noexcept;
        constexpr void swap(Iterator& o) noexcept;
        template <typename O>
        [[nodiscard]] constexpr O* cast() noexcept;
    };

    constexpr PtrRange(T* ptr, std::size_t count, std::size_t jump) noexcept;

    [[nodiscard]] constexpr Iterator begin() noexcept;
    [[nodiscard]] constexpr Iterator end() noexcept;
};
}

template <typename T>
constexpr gearoenix::core::PtrRange<T>::Iterator::Iterator(T* const ptr, const std::size_t jump, T* const end_ptr) noexcept
    : ptr(ptr)
    , jump(jump)
    , end_ptr(end_ptr)
{
}

template <typename T>
constexpr gearoenix::core::PtrRange<T>::Iterator::Iterator(const Iterator& o) noexcept
    : ptr(o.ptr)
    , jump(o.jump)
    , end_ptr(o.end_ptr)
{
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator& gearoenix::core::PtrRange<T>::Iterator::operator=(const Iterator& o) noexcept
{
    if (this == &o)
        return *this;
    ptr = o.ptr;
    jump = o.jump;
    end_ptr = o.end_ptr;
    return *this;
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator& gearoenix::core::PtrRange<T>::Iterator::operator++() noexcept
{
    ptr += jump;
    if (end_ptr < ptr)
        ptr = end_ptr;
    return *this;
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator::reference gearoenix::core::PtrRange<T>::Iterator::operator*() noexcept
{
    return ptr;
}

template <typename T>
template <typename I>
constexpr typename gearoenix::core::PtrRange<T>::Iterator& gearoenix::core::PtrRange<T>::Iterator::operator+=(const I increment) noexcept
{
    ptr += increment * jump;
    if (end_ptr < ptr)
        ptr = end_ptr;
    return *this;
}

template <typename T>
template <typename I>
constexpr typename gearoenix::core::PtrRange<T>::Iterator gearoenix::core::PtrRange<T>::Iterator::operator+(const I increment) const noexcept
{
    auto new_ptr = ptr + (increment * jump);
    if (end_ptr < new_ptr)
        new_ptr = end_ptr;
    return Iterator(new_ptr, jump, end_ptr);
}

template <typename T>
constexpr bool gearoenix::core::PtrRange<T>::Iterator::operator==(const Iterator& o) const noexcept
{
    return ptr == o.ptr && jump == o.jump && end_ptr == o.end_ptr;
}

template <typename T>
constexpr bool gearoenix::core::PtrRange<T>::Iterator::operator!=(const Iterator& o) const noexcept
{
    return ptr != o.ptr || jump != o.jump || end_ptr != o.end_ptr;
}

template <typename T>
constexpr void gearoenix::core::PtrRange<T>::Iterator::swap(Iterator& o) noexcept
{
    std::swap(ptr, o.ptr);
    std::swap(jump, o.jump);
    std::swap(end_ptr, o.end_ptr);
}

template <typename T>
template <typename O>
constexpr O* gearoenix::core::PtrRange<T>::Iterator::cast() noexcept
{
    return reinterpret_cast<O*>(ptr);
}

template <typename T>
constexpr gearoenix::core::PtrRange<T>::PtrRange(T* const ptr, const std::size_t count, const std::size_t jump) noexcept
    : ptr(ptr)
    , count(count)
    , jump(jump)
    , end_ptr(ptr + count)
{
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator gearoenix::core::PtrRange<T>::begin() noexcept
{
    return Iterator(ptr, jump, end_ptr);
}

template <typename T>
constexpr typename gearoenix::core::PtrRange<T>::Iterator gearoenix::core::PtrRange<T>::end() noexcept
{
    return Iterator(end_ptr, jump, end_ptr);
}

#endif