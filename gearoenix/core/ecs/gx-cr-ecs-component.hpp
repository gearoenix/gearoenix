#pragma once
#include "../allocator/gx-cr-alc-shared-array.hpp"
#include "../gx-cr-type-index-set.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-types.hpp"
#include <any>
#include <array>
#include <boost/container/flat_map.hpp>
#include <boost/core/demangle.hpp>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::core::ecs {
struct World;
struct Component {
    typedef TypeIndexSet<2 /*increase this based on your project needs*/> TypeIndexSet;
    typedef TypeIndexSet::element_t TypeIndex;

    struct TypeInfo final {
        GX_GET_CREF_PRV(std::string, name);
        GX_GET_CREF_PRV(std::any, allocator);
        GX_GET_CREF_PRV(TypeIndexSet, all_parents);
        GX_GET_CREF_PRV(TypeIndexSet, immediate_parents);
        GX_GET_VAL_PRV(bool, is_final, false);

    public:
        TypeInfo() = default;

        TypeInfo(
            std::string&& name,
            std::any&& allocator,
            const TypeIndexSet& all_parents,
            const TypeIndexSet& immediate_parents,
            const bool is_final)
            : name(std::move(name))
            , allocator(std::move(allocator))
            , all_parents(all_parents)
            , immediate_parents(immediate_parents)
            , is_final(is_final)
        {
        }
    };

    typedef std::array<TypeInfo, TypeIndexSet::bits_count> TypesInfos;

    GX_GET_REFC_PRT(TypeIndex, final_type_index);
    GX_GET_REFC_PRT(std::string, name);
    GX_GETSET_VAL_PRT(bool, enabled, true);
    GX_GET_CREF_PRT(std::weak_ptr<Component>, component_self);
    GX_GETSET_VAL_PRT(entity_id_t, entity_id, INVALID_ENTITY_ID);

private:
    static TypesInfos types_infos;
    static boost::container::flat_map<std::string, TypeIndex> type_name_to_index;

    static void register_type(TypeIndex t, TypeInfo&&);

public:
    Component(TypeIndex final_type_index, std::string&& name, entity_id_t entity_id);
    virtual ~Component() = default;
    Component(Component&&) = delete;
    Component(const Component&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(const Component&) = delete;

    virtual void show_debug_gui(const render::engine::Engine&);

    template <typename T>
    using clean_t = not_t<std::remove_pointer_t<std::remove_cvref_t<T>>>;

    template <typename T>
    constexpr static void type_check()
    {
        static_assert(!std::is_reference_v<T>, "Component type can not be a reference type.");
        static_assert(!std::is_move_assignable_v<T>, "Component type can not be move assignable.");
        static_assert(!std::is_move_constructible_v<T>, "Component type must be move constructable.");
        static_assert(!std::is_nothrow_move_constructible_v<T>, "Component type must be nothrow move constructable.");
        static_assert(!std::is_copy_assignable_v<T>, "Component type can not be copy assignable.");
        static_assert(!std::is_copy_constructible_v<T>, "Component type can not be copy constructable.");
        static_assert(std::is_final_v<T>, "Component type must be final and has no child");
        static_assert(std::is_base_of_v<Component, T>, "Component type must be inherited from gearoenix::core::ecs::Component");
        static_assert(!is_not_v<T>, "Component type can not be gearoenix::core::ecs::Not");
        static_assert(!is_all_v<T>, "Component type can not be gearoenix::core::ecs::And");
        static_assert(!is_any_v<T>, "Component type can not be gearoenix::core::ecs::Or");
    }

    template <typename... T>
    constexpr static void types_check()
    {
        ((type_check<T>()), ...);
    }

    template <typename Tuple, std::uint32_t... I>
    constexpr static void tuple_types_check(std::index_sequence<I...> const&)
    {
        ((type_check<std::tuple_element_t<I, Tuple>>()), ...);
    }

    template <typename Condition>
    constexpr static void condition_types_check()
    {
        tuple_types_check<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>());
    }

    template <typename T>
    constexpr static TypeIndex create_type_index()
    {
        return clean_t<T>::TYPE_INDEX;
    }

    template <typename T>
    constexpr static TypeIndex create_this_type_index(const T* const)
    {
        using DT = clean_t<T>;
        type_check<DT>();
        return create_type_index<DT>();
    }

    template <typename T>
    static std::string create_this_type_name(const T* const)
    {
        using DT = clean_t<T>;
        type_check<DT>();
        return boost::core::demangle(typeid(DT).name());
    }

    template <typename T, typename ComponentDerivedType = T /*specify it for those types that don't have any root in component*/>
    static void register_type()
    {
        static_assert(std::is_base_of_v<Component, ComponentDerivedType>);
        static_assert(std::is_base_of_v<T, ComponentDerivedType>);
        static_assert(!std::is_same_v<T, Component>);
        static_assert(create_type_index<T>() < TypeIndexSet::bits_count);
        register_type(
            create_type_index<T>(),
            { boost::core::demangle(typeid(clean_t<T>).name()),
                std::conditional_t<std::is_final_v<T>, allocator::SharedArray<T, T::MAX_COUNT>, allocator::SharedArray<char, 0>>::construct(),
                T::ALL_PARENT_TYPE_INDICES,
                T::IMMEDIATE_PARENT_TYPE_INDICES,
                std::is_final_v<T> });
    }

    static const TypesInfos& get_types_infos() { return types_infos; }
    static const TypeInfo& get_type_info(const TypeIndex ti) { return types_infos[ti]; }
    template <typename T>
    static const TypeInfo& get_type_info() { return types_infos[create_type_index<T>()]; }

    static const boost::container::flat_map<std::string, TypeIndex>& get_type_name_to_index() { return type_name_to_index; }
    static std::optional<TypeIndex> get_type_index(const std::string& type_name);

    template <typename T, typename... Args>
    [[nodiscard]] static std::shared_ptr<T> construct(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>);
        static_assert(std::is_final_v<T>);
        auto ptr = std::any_cast<const std::shared_ptr<allocator::SharedArray<T, T::MAX_COUNT>>&>(get_type_info<T>().get_allocator())->make_shared(std::forward<Args>(args)...);
        static_cast<Component*>(ptr.get())->component_self = ptr;
        return ptr;
    }
};
}