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
    typedef entity_id_t id_t;

    struct Builder final {
        friend struct World;

    private:
        const id_t id;
        std::map<std::type_index, std::vector<std::uint8_t>> components;

        explicit Builder(id_t) noexcept;

    public:
        Builder() noexcept;
        Builder(Builder&&) noexcept;
        Builder(const Builder&) = delete;
        Builder& operator=(Builder&&) = delete;
        Builder& operator=(const Builder&) = delete;

        template <typename ComponentType>
        void add_component(ComponentType&& component) noexcept
        {
            Component::types_check<ComponentType>();
            const std::type_index type_index(typeid(ComponentType));
#ifdef GX_DEBUG_MODE
            if (components.contains(type_index))
                GX_LOG_F("Component '" << typeid(ComponentType).name() << "' already exists in entity '" << id)
#endif
            std::vector<std::uint8_t> data(sizeof(ComponentType));
            new (data.data()) ComponentType(std::forward<ComponentType>(component));
            components.emplace(type_index, std::move(data));
        }

        template <typename... ComponentType>
        void add_components(ComponentType&&... cs) noexcept
        {
            ((add_component(std::move(cs))), ...);
        }
    };

private:
    Entity(archetype_id_t&& archetype, std::size_t index_in_archetype) noexcept;
    static std::atomic<id_t> last_id;
    archetype_id_t archetype;
    std::size_t index_in_archetype;

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) = delete;
    Entity& operator=(const Entity&) = delete;
};
}

#endif