#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP
#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-entity.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>
#include <variant>

namespace gearoenix::core::ecs {
/// The World of ECS
///
/// Main focus of this struct is performance of systems and memory usage
struct World final {
private:
    boost::container::flat_map<Archetype::id_t, std::size_t> archetypes_map;
    std::vector<Archetype> archetypes;
    // id -> (archetype, index)
    boost::container::flat_map<Entity::id_t, Entity> entities;
    boost::container::flat_map<std::string, Entity::id_t> name_to_entity_id;

    std::mutex delayed_actions_lock;
    // 0 - Entity addition
    // 1 - Entity deletion
    // 2 - Component(s) addition
    // 3 - Component(s) deletion
    std::vector<std::variant<
        EntityBuilder,
        Entity::id_t,
        std::pair<Entity::id_t, EntityBuilder::components_t>,
        std::pair<Entity::id_t, std::vector<std::type_index>>>>
        delayed_actions;

public:
    World() noexcept = default;
    //--------------------------------------Entity creation----------------------------------------------
    /// You must know your context (state of world), unless you want to end up having race
    void create_entity_with_builder(EntityBuilder&&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] Entity::id_t create_entity(ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        auto archetype_id = Archetype::create_id<ComponentsTypes...>();
        const auto id = ++Entity::last_id;
        auto search = archetypes_map.find(archetype_id);
        if (archetypes_map.end() == search) {
            bool is_ok;
            std::tie(search, is_ok) = archetypes_map.emplace(std::move(archetype_id), archetypes.size());
            if (!is_ok) {
                GX_LOG_F("Problem in allocation of archetype");
            }
            archetypes.push_back(Archetype::create<ComponentsTypes...>());
        }
        const auto ai = search->second;
        const auto index = archetypes[ai].allocate(id, std::move(components)...);
        entities.emplace(id, Entity(ai, index, std::nullopt));
        return id;
    }

    /// Recommended way to add an entity, in case you do not know the context you're in.
    void delayed_create_entity_with_builder(EntityBuilder&&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] Entity::id_t delayed_create_entity(ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        EntityBuilder b;
        b.add_components(std::forward<ComponentsTypes>(components)...);
        const auto id = b.get_id();
        delayed_create_entity_with_builder(std::move(b));
        return id;
    }
    //--------------------------------------Entity deletion----------------------------------------------
    void remove_entity(Entity::id_t) noexcept;

    void delayed_remove_entity(Entity::id_t) noexcept;
    //--------------------------------------Component addition-------------------------------------------
    void add_components_map(Entity::id_t, EntityBuilder::components_t&&) noexcept;

    template <typename... ComponentsTypes>
    void add_components(const Entity::id_t ei, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        EntityBuilder b(ei);
        b.add_components(std::forward<ComponentsTypes>(components)...);
        add_components_map(ei, std::move(b.components));
    }

    void delayed_add_components_map(Entity::id_t, EntityBuilder::components_t&&) noexcept;

    template <typename... ComponentsTypes>
    void delayed_add_components(const Entity::id_t ei, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        EntityBuilder b(ei);
        b.add_components(std::forward<ComponentsTypes>(components)...);
        delayed_add_components_map(ei, std::move(b.components));
    }
    //--------------------------------------Component deletion-------------------------------------------
    template <typename... ComponentsTypes>
    void remove_components(const Entity::id_t ei) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        const std::type_index ts[] = {
            Component::create_type_index<ComponentsTypes>()...,
        };
        remove_components_list(ei, ts, sizeof...(ComponentsTypes));
    }

    void remove_components_list(Entity::id_t, const std::type_index*, std::size_t) noexcept;

    template <typename... ComponentsTypes>
    void delayed_remove_components(const Entity::id_t ei) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        delayed_remove_components_list(
            ei,
            {
                Component::create_type_index<ComponentsTypes>()...,
            });
    }

    void delayed_remove_components_list(Entity::id_t, std::vector<std::type_index>&&) noexcept;

    /// It is better to not use it very much.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component(const Entity::id_t id) noexcept
    {
        Component::type_check<ComponentType>();
        auto entity_search = entities.find(id);
        if (entities.end() == entity_search)
            return nullptr;
        const auto& e = entity_search->second;
        return archetypes[e.archetype].get_component<ComponentType>(e.index_in_archetype);
    }

    /// It is better to not use it very much.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename ComponentType>
    [[nodiscard]] const ComponentType* get_component(const Entity::id_t id) const noexcept
    {
        Component::type_check<ComponentType>();
        const auto entity_search = entities.find(id);
        if (entities.end() == entity_search)
            return nullptr;
        const auto& e = entity_search->second;
        return archetypes[e.archetype].get_component<ComponentType>(e.index_in_archetype);
    }

    /// It is better to not use it very much.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename... ComponentTypes>
    [[nodiscard]] std::tuple<ComponentTypes*...> get_components(const Entity::id_t id) noexcept
    {
        Component::types_check<ComponentTypes...>();
        auto entity_search = entities.find(id);
        if (entities.end() == entity_search)
            return {
                reinterpret_cast<ComponentTypes*>(0)...,
            };
        const auto& e = entity_search->second;
        auto& a = archetypes[e.archetype];
        return {
            a.get_component<ComponentTypes>(e.index_in_archetype)...,
        };
    }

    /// It is better to not use it very much.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename... ComponentTypes>
    [[nodiscard]] std::tuple<const ComponentTypes*...> get_components(const Entity::id_t id) const noexcept
    {
        Component::types_check<ComponentTypes...>();
        auto entity_search = entities.find(id);
        if (entities.end() == entity_search)
            return {
                static_cast<const ComponentTypes*>(nullptr)...,
            };
        const auto& e = entity_search->second;
        const auto& a = archetypes[e.archetype];
        return {
            a.get_component<ComponentTypes>(e.index_in_archetype)...,
        };
    }

    [[nodiscard]] std::size_t get_archetype_index(const EntityBuilder::components_t& cs) noexcept;

    [[nodiscard]] Entity* get_entity(Entity::id_t) noexcept;
    [[nodiscard]] const Entity* get_entity(Entity::id_t) const noexcept;

    /// Highly optimized way of system execution
    template <typename Condition, typename F>
    void parallel_system(F&& fun) noexcept
    {
        Component::condition_types_check<Condition>();
        for (auto& archetype : archetypes) {
            if (!archetype.satisfy<Condition>())
                continue;
            archetype.parallel_system_conditioned<Condition>(fun);
        }
    }

    /// Less performant way of system execution.
    /// Good for highly conflicting systems, can not be executed in parallel way.
    template <typename Condition, typename F>
    void synchronised_system(F&& fun) noexcept
    {
        Component::condition_types_check<Condition>();
        for (auto& archetype : archetypes) {
            if (!archetype.satisfy<Condition>())
                continue;
            archetype.synchronised_system_conditioned<Condition>(fun);
        }
    }

    /// It will do all the delayed actions
    void update() noexcept;

    void update_entity(std::optional<std::pair<Entity::id_t, std::size_t>>&&) noexcept;

    [[nodiscard]] std::shared_ptr<EntitySharedBuilder> create_shared_builder() noexcept;
};
}

#endif