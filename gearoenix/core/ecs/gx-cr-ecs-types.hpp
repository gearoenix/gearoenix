#pragma once
#include "../gx-cr-type-index-set.hpp"
#include <cstdint>

namespace gearoenix::core::ecs {
typedef std::uint32_t entity_id_t;
typedef TypeIndexSet<2 /*increase this based on your project needs*/> component_index_set_t;
typedef component_index_set_t::element_t component_index_t;
constexpr auto invalid_entity_id = static_cast<entity_id_t>(-1);
}