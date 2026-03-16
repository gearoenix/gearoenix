#pragma once
#include "../gx-cr-object.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-condition.hpp"

namespace gearoenix::core::ecs {
struct ComponentType final {
    constexpr static auto count = component_index_set_t::bits_count;

    struct Info final {
        GX_GET_CREF_PRV(component_index_set_t, all_parents);
        GX_GET_CREF_PRV(component_index_set_t, immediate_parents);

    public:
        Info() = default;

        template <std::size_t N1, std::size_t N2>
        constexpr Info(const std::array<object_type_index_t, N1>& all_parents_oti, const std::array<object_type_index_t, N2>& immediate_parents_oti)
        {
            for (const auto ti : all_parents_oti) {
                all_parents.add(static_cast<component_index_t>(ti));
            }
            for (const auto ti : immediate_parents_oti) {
                immediate_parents.add(static_cast<component_index_t>(ti));
            }
        }
    };

    typedef std::vector<Info> Infos;

private:
    static Infos infos;

    static void add(object_type_index_t t, Info&&);

public:
    ComponentType() = delete;

    template <typename... T>
    constexpr static void check()
    {
        static_assert((!std::is_reference_v<T> && ...), "Component type can not be a reference type.");
        static_assert((!std::is_move_assignable_v<T> && ...), "Component type can not be move assignable.");
        static_assert((!std::is_move_constructible_v<T> && ...), "Component type must be move constructable.");
        static_assert((!std::is_nothrow_move_constructible_v<T> && ...), "Component type must be nothrow move constructable.");
        static_assert((!std::is_copy_assignable_v<T> && ...), "Component type can not be copy assignable.");
        static_assert((!std::is_copy_constructible_v<T> && ...), "Component type can not be copy constructable.");
        static_assert((std::is_final_v<T> && ...), "Component type must be final and has no child");
        static_assert((std::is_base_of_v<Component, T> && ...), "Component type must be inherited from gearoenix::core::ecs::Component");
        static_assert((!is_not_v<T> && ...), "Component type can not be gearoenix::core::ecs::Not");
        static_assert((!is_all_v<T> && ...), "Component type can not be gearoenix::core::ecs::And");
        static_assert((!is_any_v<T> && ...), "Component type can not be gearoenix::core::ecs::Or");
    }

    template <typename Tuple, std::uint32_t... I>
    constexpr static void tuple_check(std::index_sequence<I...> const&) { ((check<std::tuple_element_t<I, Tuple>>()), ...); }

    template <typename Condition>
    constexpr static void condition_check() { tuple_check<typename ConditionTypesPack<Condition>::types>(std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>()); }

    template <typename T>
    [[nodiscard]] constexpr static object_type_index_t create_index() { return T::object_type_index; }

    template <typename T>
    [[nodiscard]] constexpr static object_type_index_t create_index(const T* const)
    {
        check<T>();
        return create_index<T>();
    }

    /// Specify `ComponentDerivedType` for those types that don't have any root in a component, but they have a derived struct that has.
    template <typename T, typename ComponentDerivedType = T>
    static void add()
    {
        static_assert(std::is_base_of_v<Component, ComponentDerivedType>);
        static_assert(std::is_base_of_v<T, ComponentDerivedType>);
        static_assert(!std::is_same_v<T, Component>);
        static_assert(create_index<T>() < count);
        static_assert(T::max_count > 0);
        Object::register_type<T, ComponentDerivedType>();
        add(create_index<T>(), Info(T::all_parent_object_type_indices, T::immediate_parent_object_type_indices));
    }

    [[nodiscard]] static const Info& get_info(object_type_index_t ti);
    [[nodiscard]] static bool is_registered(object_type_index_t ti);
    template <typename T>
    [[nodiscard]] static const Info& get_info() { return infos[create_index<T>()]; }
    [[nodiscard]] static std::optional<object_type_index_t> get_index(const std::string& type_name);
    [[nodiscard]] static bool check(object_type_index_t ti);
};

}
