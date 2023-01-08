#ifndef GEAROENIX_CORE_ECS_COMPONENT_HPP
#define GEAROENIX_CORE_ECS_COMPONENT_HPP
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-condition.hpp"
#include <boost/container/flat_map.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <typeindex>

namespace gearoenix::core::ecs {
struct Component {
    struct TypeInfo final {
        typedef std::function<void(void* /*dst*/, void* /*src*/)> MoveConstructor;

        GX_GET_CREF_PRV(MoveConstructor, move_constructor);
        GX_GET_VAL_PRV(std::size_t, size, static_cast<std::size_t>(-1));
        GX_GET_CREF_PRV(std::string, name);

    public:
        TypeInfo(MoveConstructor&& move_constructor, const std::size_t size, std::string&& name) noexcept
            : move_constructor(std::move(move_constructor))
            , size(size)
            , name(std::move(name))
        {
        }
        TypeInfo(TypeInfo&&) noexcept = default;
        TypeInfo& operator=(TypeInfo&&) noexcept = default;
    };

    GX_GET_CREF_PRT(std::type_index, type_index);
    GX_GET_CREF_PRT(std::string, name);

public:
    bool enabled = true;

private:
    static boost::container::flat_map<std::type_index, TypeInfo> type_infos;

public:
    template <typename T>
    static void register_type() noexcept
    {
        type_infos.emplace(
            Component::create_type_index<T>(),
            TypeInfo(
                [](void* const d, void* const s) noexcept { new (d) T(std::move(*reinterpret_cast<T*>(s))); },
                sizeof(T),
                std::string(typeid(T).name())));
    }

    [[nodiscard]] static const TypeInfo& get_type_info(const std::type_index& ti) noexcept;

    template <typename T>
    explicit Component(T* const, std::string&& name) noexcept
        : type_index(Component::create_type_index<T>())
        , name(std::move(name))
    {
#ifdef GX_DEBUG_MODE
        (void)get_type_info(type_index);
#endif
    }

    virtual ~Component() noexcept = default;
    Component(Component&&) noexcept = default;
    Component(const Component&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(const Component&) = delete;

    virtual void display() noexcept { }

    template <typename T>
    constexpr static void type_check() noexcept
    {
        static_assert(!std::is_reference_v<T>, "Component type can not be a reference type.");
        static_assert(!std::is_move_assignable_v<T>, "Component type can not be move assignable.");
        static_assert(std::is_move_constructible_v<T>, "Component type must be move constructable.");
        static_assert(std::is_nothrow_move_constructible_v<T>, "Component type must be nothrow move constructable.");
        static_assert(!std::is_copy_assignable_v<T>, "Component type can not be copy assignable.");
        static_assert(!std::is_copy_constructible_v<T>, "Component type can not be copy constructable.");
        static_assert(std::is_final_v<T>, "Component type must be final and has no child");
        static_assert(std::is_base_of_v<Component, T>, "Component type must be inherited from gearoenix::core::ecs::Component");
        static_assert(!is_not_v<T>, "Component type can not be gearoenix::core::ecs::Not");
        static_assert(!is_and_v<T>, "Component type can not be gearoenix::core::ecs::And");
        static_assert(!is_or_v<T>, "Component type can not be gearoenix::core::ecs::Or");
    }

    template <typename... T>
    constexpr static void types_check() noexcept
    {
        ((type_check<T>()), ...);
    }

    template <typename Tuple, std::size_t... I>
    constexpr static void tuple_types_check(std::index_sequence<I...> const&) noexcept
    {
        ((type_check<std::tuple_element_t<I, Tuple>>()), ...);
    }

    template <typename Condition>
    constexpr static void condition_types_check() noexcept
    {
        tuple_types_check<typename ConditionTypesPack<Condition>::types>(
            std::make_index_sequence<std::tuple_size_v<typename ConditionTypesPack<Condition>::types>>());
    }

    template <typename T>
    static std::type_index create_type_index() noexcept
    {
        type_check<T>();
        return std::type_index(typeid(not_t<T>));
    }
};
}
#endif