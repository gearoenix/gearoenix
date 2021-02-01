#ifndef GEAROENIX_CORE_ECS_ARCHETYPE_HPP
#define GEAROENIX_CORE_ECS_ARCHETYPE_HPP
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

    constexpr static flag_t deleted = 1;

    constexpr static std::size_t header_size = sizeof(flag_t) + sizeof(entity_id_t);
    const std::size_t components_size;
    const std::map<std::type_index, std::size_t> components_indices;
    const std::size_t entity_size;

    std::vector<std::uint8_t> data;

    template <typename... ComponentsTypes>
    [[nodiscard]] static id_t create_id() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        return {
            std::type_index(typeid(ComponentsTypes))...,
        };
    }

    template <typename... ComponentsTypes>
    [[nodiscard]] static bool satisfy(const id_t& id) noexcept
    {
        Component::query_types_check<ComponentsTypes...>();
        bool result = true;
        (([&]<typename T>(T*) noexcept {
            if (result) {
                if (IsNot<T>::value) {
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
    [[nodiscard]] static std::map<std::type_index, std::size_t> get_components_indices() noexcept
    {
        Component::types_check<ComponentsTypes...>();
        std::size_t index = 0;
        std::map<std::type_index, std::size_t> indices;
        (([&]<typename T>(T*) constexpr noexcept {
            indices[std::type_index(typeid(T))] = index;
            index += sizeof(T);
        }(reinterpret_cast<std::remove_reference_t<ComponentsTypes>*>(0))),
            ...);
        return indices;
    }

    [[nodiscard]] static std::map<std::type_index, std::size_t> get_components_indices(
        const id_t&, const std::vector<std::vector<std::uint8_t>>&) noexcept;

    Archetype(const id_t&, const std::vector<std::vector<std::uint8_t>>&) noexcept;
    Archetype(std::size_t, std::map<std::type_index, std::size_t>) noexcept;

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
            const auto index = components_indices.find(std::type_index(typeid(T)))->second;
            auto* const cp = reinterpret_cast<T*>(&ptr[index]);
            new (cp) T(std::forward<T>(component));
        }(std::move(components))),
            ...);
        return result;
    }

    void delete_entity(std::size_t index) noexcept;

    template <typename ComponentType>
    [[nodiscard]] ComponentType& get_component(const std::size_t index) noexcept
    {
        Component::type_check<ComponentType>();
        return *reinterpret_cast<ComponentType*>(&data[index + components_indices.find(std::type_index(typeid(ComponentType)))->second]);
    }

    template <typename... ComponentsTypes>
    void parallel_system(const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept
    {
        Component::query_types_check<ComponentsTypes...>();
        const std::size_t indices[] = {
            (IsNot<ComponentsTypes>::value ? 0 : components_indices.find(std::type_index(typeid(ComponentsTypes)))->second)...,
        };
        auto range = PtrRange(data.data(), data.size(), entity_size);
        std::for_each(std::execution::par_unseq, range.begin(), range.end(), [&](std::uint8_t* ptr) {
            const auto flag = *reinterpret_cast<const flag_t*>(ptr);
            if (deleted == (deleted & flag))
                return;
            ptr += sizeof(flag_t);
            const auto entity_id = *reinterpret_cast<const entity_id_t*>(ptr);
            ptr += sizeof(entity_id_t);
            auto index = static_cast<std::size_t>(-1);
            fun(entity_id, *reinterpret_cast<ComponentsTypes*>(&ptr[indices[++index]])...);
        });
    }

public:
    Archetype(Archetype&&) noexcept;
    Archetype(const Archetype&) = delete;
};
}

#endif