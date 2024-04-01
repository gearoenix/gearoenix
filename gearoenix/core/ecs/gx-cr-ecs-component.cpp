#include "gx-cr-ecs-component.hpp"

gearoenix::core::ecs::Component::Component(const std::type_index final_type_index, std::string&& name)
    : final_type_index(final_type_index)
    , name(std::move(name))
{
}

void gearoenix::core::ecs::Component::set_component_self(const std::shared_ptr<Component>& c)
{
    component_self = c;
}
