#pragma once
#include "../gx-cr-type-index-set.hpp"

namespace gearoenix::core::ecs {
typedef TypeIndexSet<2 /*increase this based on your project needs*/> component_index_set_t;
typedef component_index_set_t::element_t component_index_t;
}