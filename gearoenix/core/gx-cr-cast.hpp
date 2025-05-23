#pragma once
#include "../platform/gx-plt-log.hpp"
#include <memory>

namespace gearoenix::core {
template <typename T, typename I>
constexpr T* cast(I* const ptr)
{
    GX_ASSERT_D(dynamic_cast<T*>(ptr) == static_cast<T*>(ptr));
    return static_cast<T*>(ptr);
}

template <typename T, typename I>
constexpr std::shared_ptr<T> cast(std::shared_ptr<I>&& ptr)
{
    GX_ASSERT_D(std::dynamic_pointer_cast<T>(ptr) == std::static_pointer_cast<T>(ptr));
    return std::static_pointer_cast<T>(std::move(ptr));
}
}