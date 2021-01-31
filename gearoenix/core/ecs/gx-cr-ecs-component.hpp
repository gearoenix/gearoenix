#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include <cstdint>

namespace gearoenix::core::ecs {
struct Component {
    bool enabled = true;

    virtual ~Component() noexcept = default;
    virtual void display() noexcept { }
};
}

#endif