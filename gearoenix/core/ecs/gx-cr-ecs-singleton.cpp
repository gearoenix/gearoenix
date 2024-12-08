#include "gx-cr-ecs-singleton.hpp"

boost::container::flat_map<gearoenix::core::ecs::component_index_t, std::shared_ptr<gearoenix::core::ecs::Component>> gearoenix::core::ecs::Singleton::values;