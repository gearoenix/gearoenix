#include "gx-cr-ecs-component.hpp"
#include "../../platform/gx-plt-log.hpp"

boost::container::flat_map<std::type_index, gearoenix::core::ecs::Component::TypeInfo> gearoenix::core::ecs::Component::type_infos = {};

const gearoenix::core::ecs::Component::TypeInfo& gearoenix::core::ecs::Component::get_type_info(const std::type_index& ti) noexcept {
    const auto search = type_infos.find(ti);
    if(type_infos.end() == search) {
        GX_LOG_F("Queries type does not exist in the registered list of component types, you forgot to register it.");
    }
    return search->second;
}