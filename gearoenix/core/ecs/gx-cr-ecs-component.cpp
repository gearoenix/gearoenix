#include "gx-cr-ecs-component.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"

boost::container::flat_map<std::type_index, std::string> gearoenix::core::ecs::Component::type_index_to_name;

boost::container::flat_map<std::string, std::type_index> gearoenix::core::ecs::Component::type_name_to_index;

void gearoenix::core::ecs::Component::register_type(std::type_index t, std::string name)
{
#if GX_DEBUG_MODE
    if (const auto search = type_index_to_name.find(t); type_index_to_name.end() != search) {
        GX_ASSERT_D(search->second == name);
    }
    if (const auto search = type_name_to_index.find(name); type_name_to_index.end() != search) {
        GX_ASSERT_D(search->second == t);
    }
#endif
    type_index_to_name.emplace(t, name);
    type_name_to_index.emplace(std::move(name), t);
}

gearoenix::core::ecs::Component::Component(const std::type_index final_type_index, std::string&& name, const entity_id_t entity_id)
    : final_type_index(final_type_index)
    , name(std::move(name))
    , entity_id(entity_id)
{
}

void gearoenix::core::ecs::Component::set_component_self(const std::shared_ptr<Component>& c)
{
    component_self = c;
}
