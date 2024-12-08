#pragma once
#include "../allocator/gx-cr-alc-shared-array.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-condition.hpp"
#include <any>
#include <boost/container/flat_map.hpp>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::core::ecs {
template <typename T>
concept HasReadConstructor = requires(std::string&& n, const entity_id_t id, std::shared_ptr<platform::stream::Stream>&& s, job::EndCallerShared<Component>&& c) {
    { T::construct(std::move(n), id, std::move(s), std::move(c)) } -> std::same_as<void>;
};

struct ComponentType final {
    friend struct Singleton;

    typedef std::function<void(std::string&&, entity_id_t, std::shared_ptr<platform::stream::Stream>&&, job::EndCallerShared<Component>&&)> StreamConstructor;

    constexpr static auto count = component_index_set_t::bits_count;

    struct Info final {
        GX_GET_CREF_PRV(std::string, name);
        GX_GET_CREF_PRV(std::any, allocator);
        GX_GET_CREF_PRV(component_index_set_t, all_parents);
        GX_GET_CREF_PRV(component_index_set_t, immediate_parents);
        GX_GET_VAL_PRV(bool, is_final, false);
        GX_GET_VAL_PRV(std::uint32_t, max_count, 1);
        GX_GET_CREF_PRV(StreamConstructor, construct_stream);

    public:
        Info() = default;

        Info(
            std::string&& name,
            std::any&& allocator,
            const component_index_set_t& all_parents,
            const component_index_set_t& immediate_parents,
            const bool is_final,
            const std::uint32_t max_count,
            StreamConstructor&& construct_stream)
            : name(std::move(name))
            , allocator(std::move(allocator))
            , all_parents(all_parents)
            , immediate_parents(immediate_parents)
            , is_final(is_final)
            , max_count(max_count)
            , construct_stream(std::move(construct_stream))
        {
        }
    };

    typedef std::array<Info, count> Infos;

private:
    static Infos infos;
    static boost::container::flat_map<std::string, component_index_t> name_to_index;

    static void add(component_index_t t, Info&&);

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
    constexpr static void tuple_check(std::index_sequence<I...> const&)
    {
        ((check<std::tuple_element_t<I, Tuple>>()), ...);
    }

    template <typename Condition>
    constexpr static void condition_check()
    {
        tuple_check<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>());
    }

    template <typename T>
    [[nodiscard]] constexpr static component_index_t create_index()
    {
        return T::TYPE_INDEX;
    }

    template <typename T>
    [[nodiscard]] constexpr static component_index_t create_index(const T* const)
    {
        check<T>();
        return create_index<T>();
    }

    template <typename T>
    [[nodiscard]] static std::string create_name(const T* const)
    {
        return boost::core::demangle(typeid(T).name());
    }

    /// Specify `ComponentDerivedType` for those types that don't have any root in component but, they have a derived structs that have.
    template <typename T, typename ComponentDerivedType = T>
    static void add()
    {
        static_assert(std::is_base_of_v<Component, ComponentDerivedType>);
        static_assert(std::is_base_of_v<T, ComponentDerivedType>);
        static_assert(!std::is_same_v<T, Component>);
        static_assert(create_index<T>() < count);
        static_assert(T::MAX_COUNT > 0);
        add(
            create_index<T>(),
            { boost::core::demangle(typeid(T).name()),
                std::conditional_t<std::is_final_v<T>, allocator::SharedArray<T, T::MAX_COUNT>, allocator::SharedArray<char, 0>>::construct(),
                T::ALL_PARENT_TYPE_INDICES,
                T::IMMEDIATE_PARENT_TYPE_INDICES,
                std::is_final_v<T>,
                T::MAX_COUNT,
                [](
                    [[maybe_unused]] std::string&& n,
                    [[maybe_unused]] const entity_id_t i,
                    [[maybe_unused]] std::shared_ptr<platform::stream::Stream>&& s,
                    [[maybe_unused]] job::EndCallerShared<Component>&& e) {
                    if constexpr (HasReadConstructor<T> && std::is_final_v<T>) {
                        T::construct(std::move(n), i, std::move(s), std::move(e));
                    } else {
                        GX_UNEXPECTED;
                    }
                } });
    }

    [[nodiscard]] static const Info& get_info(const component_index_t ti) { return infos[ti]; }
    [[nodiscard]] static bool is_not_registered(const component_index_t ti) { return infos[ti].get_name().empty(); }
    [[nodiscard]] static bool is_registered(const component_index_t ti) { return !is_not_registered(ti); }
    template <typename T>
    [[nodiscard]] static const Info& get_info() { return infos[create_index<T>()]; }
    [[nodiscard]] static std::optional<component_index_t> get_index(const std::string& type_name);
    [[nodiscard]] static const std::string& get_name(const component_index_t ti) { return infos[ti].get_name(); }
    [[nodiscard]] static const StreamConstructor& get_stream_constructor(const component_index_t ti) { return infos[ti].get_construct_stream(); }
    [[nodiscard]] static bool check(component_index_t ti);
};
}