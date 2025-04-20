#include "gx-cr-ecs-comp-type.hpp"

gearoenix::core::ecs::ComponentType::Infos gearoenix::core::ecs::ComponentType::infos { count };

void gearoenix::core::ecs::ComponentType::add(const object_type_index_t t, Info&& info)
{
    GX_ASSERT_D(infos.size() > t);

    infos[t] = info;
}

const gearoenix::core::ecs::ComponentType::Info& gearoenix::core::ecs::ComponentType::get_info(const object_type_index_t ti)
{
    return infos[ti];
}

bool gearoenix::core::ecs::ComponentType::is_registered(const object_type_index_t ti)
{
    return Object::is_registered(ti);
}

std::optional<gearoenix::core::object_type_index_t> gearoenix::core::ecs::ComponentType::get_index(const std::string& type_name)
{
    return Object::get_index(type_name);
}

bool gearoenix::core::ecs::ComponentType::check(const object_type_index_t ti)
{
    if (ti >= count) {
        return false;
    }
    return Object::check_object_type_registration_state(ti);
}
