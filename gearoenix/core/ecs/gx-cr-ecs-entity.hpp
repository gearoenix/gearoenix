#ifndef GEAROENIX_CORE_ECS_ENTITY_HPP
#define GEAROENIX_CORE_ECS_ENTITY_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <atomic>
#include <map>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct Entity final {
    friend struct World;
    typedef std::uint32_t id_t;
    struct Builder final {
    private:
        const id_t id;
        std::map<std::type_index, std::vector<std::uint8_t>> components;

    public:
        Builder() noexcept;
        template <typename ComponentType>
        void add(ComponentType&& component) noexcept;
    };

private:
    static std::atomic<id_t> last_id;
    archetype_id_t archetype;
    std::size_t index_in_archetype;
    std::map<std::type_index, Component*> components;
};
}

template <typename ComponentType>
void gearoenix::core::ecs::Entity::Builder::add(ComponentType&& component) noexcept
{
    Component::type_check<ComponentType>();
    const std::type_index type_index(typeid(ComponentType));
#ifdef GX_DEBUG_MODE
    if (components.contains(type_index))
        GX_LOG_F("Component '" << typeid(ComponentType).name() << "' already exists in entity '" << id)
#endif
    std::vector<std::uint8_t> data(sizeof(ComponentType));
    new (data.data()) ComponentType(std::forward<ComponentType>(component));
    components.emplace(type_index, std::move(data));
}

#endif