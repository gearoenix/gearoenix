#ifndef GEAROENIX_CORE_ECS_TYPES_HPP
#define GEAROENIX_CORE_ECS_TYPES_HPP
#include <cstdint>
#include <set>
#include <typeindex>

namespace gearoenix::core::ecs {
typedef std::set<std::type_index> archetype_id_t;
typedef std::uint32_t entity_id_t;
}

#endif