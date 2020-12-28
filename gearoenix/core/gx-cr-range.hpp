#ifndef GEAROENIX_CORE_RANGE_HPP
#define GEAROENIX_CORE_RANGE_HPP

namespace gearoenix::core {
template <typename T>
struct PtrRange {
private:
    T* const ptr;
    const std::size_t count, jump;

public:
    PtrRange(T* ptr, std::size_t count, std::size_t jump) noexcept;
};
}

template <typename T>
PtrRange<T>::PtrRange(T* const ptr, std::size_t count, std::size_t jump) noexcept
{
}

#endif