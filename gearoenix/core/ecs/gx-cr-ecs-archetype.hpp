#ifndef GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#define GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../gx-cr-range.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-not.hpp"
#include "gx-cr-ecs-types.hpp"
#include <execution>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct Archetype final {
    friend struct World;

private:
    typedef archetype_id_t id_t;
    typedef std::uint8_t flag_t;
    typedef std::map<std::type_index, std::size_t> components_indices_t;

    constexpr static flag_t deleted = 1;

    constexpr static std::size_t header_size = sizeof(flag_t) + sizeof(entity_id_t);
    const std::size_t components_size;
    const components_indices_t components_indices;
    const std::size_t entity_size;

    std::vector<std::uint8_t> data;

    template <typename... ComponentsTypes>
    [[nodiscard]] static id_t create_id() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        id_t id {
            std::type_index(typeid(ComponentsTypes))...,
        };
#ifdef GX_DEBUG_MODE
        if (sizeof...(ComponentsTypes) != id.size())
            GX_LOG_F("Duplicated components are not allowed")
#endif
        return id;
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] static bool satisfy(const id_t& id) noexcept
    {
        Component::query_types_check<ComponentsTypes...>();
        bool result = true;
        (([&]<typename T>(T*) constexpr noexcept {
            if (result) {
                if (is_not<T>) {
                    result &= !id.contains(std::type_index(typeid(typename IsNot<T>::type)));
                } else {
                    result &= id.contains(std::type_index(typeid(T)));
                }
            }
        }(reinterpret_cast<ComponentsTypes*>(0))),
            ...);
        return result;
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] constexpr static std::size_t get_components_size() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        std::size_t size = 0;
        ((size += sizeof(ComponentsTypes)), ...);
        return size;
    }

    [[nodiscard]] static std::size_t get_components_size(const std::vector<std::vector<std::uint8_t>>&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] static components_indices_t get_components_indices() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        std::size_t index = 0;
        components_indices_t indices {
            { std::type_index(typeid(ComponentsTypes)), sizeof(ComponentsTypes) }...,
        };
        // TODO sort
        for (auto& i : indices) {
            auto s = i.second;
            i.second = index;
            index += s;
        }
        return indices;
    }

    [[nodiscard]] static components_indices_t get_components_indices(
        const id_t&, const std::vector<std::vector<std::uint8_t>>&) noexcept;

    Archetype(const id_t&, const std::vector<std::vector<std::uint8_t>>&) noexcept;
    Archetype(std::size_t, components_indices_t) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] static Archetype create() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        return Archetype(get_components_size<ComponentsTypes...>(), get_components_indices<ComponentsTypes...>());
    }

    [[nodiscard]] std::uint8_t* allocate_size(std::size_t) noexcept;

    template <typename T>
    T& allocate() noexcept
    {
        return *reinterpret_cast<T*>(allocate_size(sizeof(T)));
    }

    void allocate_entity(entity_id_t) noexcept;

    [[nodiscard]] std::size_t allocate_entity(entity_id_t, const std::vector<std::vector<std::uint8_t>>&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] std::size_t allocate(const entity_id_t id, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        allocate_entity(id);
        const auto result = data.size();
        auto* const ptr = allocate_size(get_components_size<ComponentsTypes...>());
        (([&]<typename T>(T&& component) noexcept {
            auto* const cp = reinterpret_cast<T*>(&ptr[get_component_index<T>()]);
            new (cp) T(std::forward<T>(component));
        }(std::move(components))),
            ...);
        return result;
    }

    void delete_entity(std::size_t index) noexcept;

    template <typename T>
    [[nodiscard]] std::size_t get_component_index() noexcept
    {
        Component::query_type_check<T>();
        const auto search = components_indices.find(std::type_index(typeid(T)));
#ifdef GX_DEBUG_MODE
        if (components_indices.end() == search)
            GX_LOG_F("Component '" << typeid(T).name() << "' not found in this archetype.")
#endif
        return search->second;
    }

    template <typename ComponentType>
    [[nodiscard]] ComponentType& get_component(const std::size_t index) noexcept
    {
        Component::type_check<ComponentType>();
        return *reinterpret_cast<ComponentType*>(&data[index + get_component_index<ComponentType>()]);
    }

    template <typename... ComponentsTypes>
    void parallel_system(const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept
    {
        const std::size_t indices[] = {
            (is_not<ComponentsTypes> ? 0 : get_component_index<ComponentsTypes>())...,
        };
        auto range = PtrRange(data.data(), data.size(), entity_size);
        std::for_each(std::execution::par_unseq, range.begin(), range.end(), [&](std::uint8_t* ptr) {
            const auto flag = *reinterpret_cast<const flag_t*>(ptr);
            if (deleted == (deleted & flag))
                return;
            ptr += sizeof(flag_t);
            const auto entity_id = *reinterpret_cast<const entity_id_t*>(ptr);
            ptr += sizeof(entity_id_t);
            std::size_t index = 0;
            auto index_finder = [&]<typename T>(T*) noexcept -> T& {
                auto c = reinterpret_cast<T*>(&ptr[indices[index]]);
                ++index;
                return *c;
            };
            fun(entity_id, index_finder(reinterpret_cast<ComponentsTypes*>(0))...);
        });
    }

public:
    Archetype(Archetype&&) noexcept;
    Archetype(const Archetype&) = delete;
};
}

#endif