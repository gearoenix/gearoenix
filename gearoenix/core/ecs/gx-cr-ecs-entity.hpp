#ifndef GEAROENIX_CORE_ECS_ENTITY_HPP
#define GEAROENIX_CORE_ECS_ENTITY_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <atomic>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct Entity final {
    friend struct World;
    typedef std::uint32_t id_t;

    struct Builder final {
        friend struct World;
        typedef std::pair<std::type_index, std::vector<std::uint8_t>> component_t;
        typedef std::vector<component_t> components_t;

    private:
        GX_GET_CVAL_PRV(id_t, id)
        components_t components;

        constexpr static auto component_less = +[](const component_t& l, const component_t& r) constexpr noexcept
        {
            return std::less()(l.first, r.first);
        };

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
            auto c = std::make_pair(
                Component::create_type_index<ComponentType>(),
                std::vector<std::uint8_t>(sizeof(ComponentType)));
#ifdef GX_DEBUG_MODE
            if (std::binary_search(components.begin(), components.end(), c, component_less))
                GX_LOG_F("Component '" << typeid(ComponentType).name() << "' already exists in entity '" << id)
#endif
            new (c.second.data()) ComponentType(std::forward<ComponentType>(component));
            components.push_back(std::move(c));
            sort(components);
        }

        template <typename... ComponentType>
        void add_components(ComponentType&&... cs) noexcept
        {
            ((add_component(std::move(cs))), ...);
        }

        static void sort(components_t&) noexcept;
    };

private:
    Entity(std::size_t archetype, std::size_t index_in_archetype) noexcept;
    static std::atomic<id_t> last_id;
    std::size_t archetype = static_cast<std::size_t>(-1);
    std::size_t index_in_archetype = static_cast<std::size_t>(-1);

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) = delete;
    Entity& operator=(const Entity&) = delete;
};
}

#endif