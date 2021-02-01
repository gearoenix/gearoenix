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

    template <ComponentConcept... ComponentsTypes>
    [[nodiscard]] static id_t create_id() noexcept;

    template <ComponentQueryConcept... ComponentsTypes>
    [[nodiscard]] static bool satisfy(const id_t&) noexcept;

    template <ComponentConcept... ComponentsTypes>
    [[nodiscard]] constexpr static std::size_t get_components_size() noexcept;
    [[nodiscard]] static std::size_t get_components_size(const std::vector<std::vector<std::uint8_t>>&) noexcept;

    template <ComponentConcept... ComponentsTypes>
    [[nodiscard]] static std::map<std::type_index, std::size_t> get_components_indices() noexcept;
    [[nodiscard]] static std::map<std::type_index, std::size_t> get_components_indices(
        const id_t&, const std::vector<std::vector<std::uint8_t>>&) noexcept;

    template <ComponentConcept... ComponentsTypes>
    explicit Archetype(ComponentsTypes&... components) noexcept;

    Archetype(const id_t&, const std::vector<std::vector<std::uint8_t>>&) noexcept;

    [[nodiscard]] std::uint8_t* allocate_size(std::size_t) noexcept;

    template <typename T>
    T& allocate() noexcept;

    void allocate_entity(entity_id_t) noexcept;

    [[nodiscard]] std::size_t allocate_entity(entity_id_t, const std::vector<std::vector<std::uint8_t>>&) noexcept;

    template <ComponentConcept... ComponentsTypes>
    [[nodiscard]] std::size_t allocate(entity_id_t, ComponentsTypes&&... components) noexcept;

    void delete_entity(std::size_t index) noexcept;

    template <ComponentConcept ComponentsType>
    [[nodiscard]] ComponentsType& get_component(std::size_t) noexcept;

    template <ComponentQueryConcept... ComponentsTypes>
    void parallel_system(const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept;
};
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
gearoenix::core::ecs::Archetype::id_t gearoenix::core::ecs::Archetype::create_id() noexcept
{
    return {
        std::type_index(typeid(std::remove_reference_t<ComponentsTypes>))...,
    };
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
constexpr std::size_t gearoenix::core::ecs::Archetype::get_components_size() noexcept
{
    std::size_t size = 0;
    (([&]<typename T>(T*) constexpr noexcept {
        size += sizeof(T);
    }(reinterpret_cast<std::remove_reference_t<ComponentsTypes>*>(0))),
        ...);
    return size;
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
std::map<std::type_index, std::size_t> gearoenix::core::ecs::Archetype::get_components_indices() noexcept
{
    std::size_t index = 0;
    std::map<std::type_index, std::size_t> indices;
    (([&]<typename T>(T*) constexpr noexcept {
        indices[std::type_index(typeid(T))] = index;
        index += sizeof(T);
    }(reinterpret_cast<std::remove_reference_t<ComponentsTypes>*>(0))),
        ...);
    return indices;
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
gearoenix::core::ecs::Archetype::Archetype(ComponentsTypes&...) noexcept
    : components_size(get_components_size<ComponentsTypes...>())
    , components_indices(get_components_indices<ComponentsTypes...>())
    , entity_size(header_size + components_size)
{
}

template <typename T>
T& gearoenix::core::ecs::Archetype::allocate() noexcept
{
    return *reinterpret_cast<T*>(allocate_size(sizeof(T)));
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
std::size_t gearoenix::core::ecs::Archetype::allocate(entity_id_t id, ComponentsTypes&&... components) noexcept
{
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

template <gearoenix::core::ecs::ComponentConcept ComponentsType>
ComponentsType& gearoenix::core::ecs::Archetype::get_component(const std::size_t index) noexcept
{
    return *reinterpret_cast<ComponentsType*>(&data[index + components_indices.find(std::type_index(typeid(ComponentsType)))->second]);
}

template <gearoenix::core::ecs::ComponentQueryConcept... ComponentsTypes>
bool gearoenix::core::ecs::Archetype::satisfy(const id_t& id) noexcept
{
    bool result = true;
    (([&]<typename T>(T*) noexcept {
        if (result) {
            if (IsNot<T>::value) {
                result &= !id.contains(std::type_index(typeid(typename IsNot<T>::type)));
            } else {
                result &= id.contains(std::type_index(typeid(T)));
            }
        }
    }(reinterpret_cast<std::remove_reference_t<ComponentsTypes>*>(0))),
        ...);
    return result;
}

template <gearoenix::core::ecs::ComponentQueryConcept... ComponentsTypes>
void gearoenix::core::ecs::Archetype::parallel_system(
    const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept
{
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
        fun(entity_id, *reinterpret_cast<std::remove_reference_t<ComponentsTypes>*>(&ptr[indices[++index]])...);
    });
}

#endif