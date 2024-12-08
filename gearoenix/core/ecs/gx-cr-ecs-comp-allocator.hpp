#pragma once
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-component.hpp"
#include <any>

namespace gearoenix::core::ecs {
template <typename T, typename... Args>
[[nodiscard]] std::shared_ptr<T> construct_component(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>);
    static_assert(std::is_final_v<T>);
    auto ptr = std::any_cast<const std::shared_ptr<allocator::SharedArray<T, T::MAX_COUNT>>&>(ComponentType::get_info<T>().get_allocator())->make_shared(std::forward<Args>(args)...);
    static_cast<Component*>(ptr.get())->component_self = ptr;
    return ptr;
}
}