#include "gx-cr-ecs-comp-type.hpp"

gearoenix::core::ecs::ComponentType::Infos gearoenix::core::ecs::ComponentType::infos {};

boost::container::flat_map<std::string, gearoenix::core::ecs::component_index_t> gearoenix::core::ecs::ComponentType::name_to_index;

void gearoenix::core::ecs::ComponentType::add(const component_index_t t, Info&& info)
{
    GX_ASSERT_D(infos.size() > t);

    auto& ti = infos[t];

    GX_ASSERT_D(!info.get_name().empty());
    if (ti.get_name() == info.get_name()) {
        return;
    }
    GX_ASSERT_D(ti.get_name().empty());
    GX_ASSERT_D(name_to_index.find(info.get_name()) == name_to_index.end());

    name_to_index.emplace(info.get_name(), t);
    ti = std::move(info);
}

std::optional<gearoenix::core::ecs::component_index_t> gearoenix::core::ecs::ComponentType::get_index(const std::string& type_name)
{
    const auto search = name_to_index.find(type_name);
    return search == name_to_index.end() ? std::nullopt : std::make_optional(search->second);
}

bool gearoenix::core::ecs::ComponentType::check(const component_index_t ti)
{
    if (ti >= count) {
        return false;
    }
    if (is_not_registered(ti)) {
        return false;
    }
    if (name_to_index.find(infos[ti].get_name()) == name_to_index.end()) {
        return false;
    }
    if (name_to_index[infos[ti].get_name()] != ti) {
        return false;
    }
    for (const auto pt : infos[ti].get_all_parents()) {
        if (is_not_registered(pt)) {
            return false;
        }
        if (name_to_index.find(infos[pt].get_name()) == name_to_index.end()) {
            return false;
        }
        if (name_to_index[infos[pt].get_name()] != pt) {
            return false;
        }
    }
    return true;
}
