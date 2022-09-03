#ifndef GEAROENIX_CORE_ECS_TYPES_HPP
#define GEAROENIX_CORE_ECS_TYPES_HPP
#include <cstdint>

namespace gearoenix::core::ecs {
typedef std::uint32_t entity_id_t;
constexpr auto INVALID_ENTITY_ID = static_cast<entity_id_t>(-1);
}

#endif