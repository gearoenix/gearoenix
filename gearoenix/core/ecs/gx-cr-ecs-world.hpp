#pragma once
#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-entity.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>
#include <variant>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core::ecs {
/// The World of ECS
///
/// Main focus of this struct is performance of systems and memory usage
struct World final {
    typedef std::function<bool()> resolver_t;

private:
    boost::container::flat_map<Archetype::id_t, std::unique_ptr<Archetype>> archetypes;
    // id -> (archetype, index)
    boost::container::flat_map<entity_id_t, Entity> entities;
    boost::container::flat_map<std::string, entity_id_t> name_to_entity_id;

    std::mutex resolvers_lock;
    std::vector<resolver_t> resolvers;

    struct Action final {
        struct CreateEntity final {
            EntityBuilder builder;
        };

        struct DeleteEntity final {
            entity_id_t id;
        };

        struct AddComponents final {
            entity_id_t id;
            EntityBuilder::components_t components;
        };

        struct DeleteComponents final {
            entity_id_t id;
            std::vector<component_index_t> component_types;
        };

        job::EndCaller<> callback;
        std::variant<CreateEntity, DeleteEntity, AddComponents, DeleteComponents> variant;
    };

    std::mutex delayed_actions_lock;
    std::vector<Action> delayed_actions;

    void read_existing_entity(entity_id_t, std::shared_ptr<platform::stream::Stream>&&, job::EndCaller<>&&);
    void read_in_io_context(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end_callback);
    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end_callback) const;

public:
    World();
    [[nodiscard]] static World* get();
    static void destroy();
    World(World&&) = delete;
    World(const World&) = delete;
    //--------------------------------------Entity creation----------------------------------------------
    /// You must know your context (state of world), unless you want to end up having race
    void create_entity(EntityBuilder&&);

    /// Recommended way to add an entity, in case you do not know the context you're in.
    void delayed_create_entity(EntityBuilder&&);
    //--------------------------------------Entity deletion----------------------------------------------
    void delete_entity(entity_id_t);

    void delayed_delete_entity(entity_id_t, job::EndCaller<>&& callback);
    //--------------------------------------Component addition-------------------------------------------
    void add_components_map(entity_id_t, EntityBuilder::components_t&&);

    template <typename... ComponentsTypes>
    void add_components(const entity_id_t ei, std::shared_ptr<ComponentsTypes>&&... components)
    {
        EntityBuilder b(ei, std::string(entities.find(ei)->second.name), job::EndCaller([] { }));
        b.add_components(std::move(components)...);
        add_components_map(ei, std::move(b.all_types_to_components));
    }

    void delayed_add_components_map(entity_id_t, EntityBuilder::components_t&&, job::EndCaller<>&& callback);

    template <typename... ComponentsTypes>
    void delayed_add_components(const entity_id_t ei, job::EndCaller<>&& callback, std::shared_ptr<ComponentsTypes>&&... components)
    {
        EntityBuilder b(ei, std::string(entities.find(ei)->second.name), job::EndCaller(callback));
        b.add_components(std::move(components)...);
        delayed_add_components_map(ei, std::move(b.all_types_to_components), std::move(callback));
    }
    //--------------------------------------Component deletion-------------------------------------------
    template <typename... ComponentsTypes>
    void remove_components(const entity_id_t ei)
    {
        ComponentType::check<ComponentsTypes...>();
        const std::array<component_index_t, sizeof...(ComponentsTypes)> ts = {
            ComponentsTypes::TYPE_INDEX...,
        };
        remove_components_list(ei, ts.data(), ts.size());
    }

    void remove_components_list(entity_id_t, const component_index_t*, std::uint64_t);

    template <typename... ComponentsTypes>
    void delayed_remove_components(const entity_id_t ei, job::EndCaller<>&& callback)
    {
        ComponentType::check<ComponentsTypes...>();
        delayed_remove_components_list(
            ei,
            {
                ComponentType::create_index<ComponentsTypes>()...,
            },
            std::move(callback));
    }

    void delayed_remove_components_list(entity_id_t, std::vector<component_index_t>&&, job::EndCaller<>&& callback);

    /// It is better to not use it too often.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component(const entity_id_t id) const
    {
        const auto search = entities.find(id);
        if (entities.end() == search) {
            return nullptr;
        }
        const auto& e = search->second;
        return e.archetype->get_component<ComponentType>(e.components);
    }

    /// It is better to not use it too often.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename ComponentType>
    [[nodiscard]] std::shared_ptr<ComponentType> get_component_shared_ptr(const entity_id_t id) const
    {
        const auto search = entities.find(id);
        if (entities.end() == search) {
            return nullptr;
        }
        const auto& e = search->second;
        return e.archetype->get_component_shared_ptr<ComponentType>(e.components);
    }

    /// It is better to not use it too often.
    /// It is recommended to design your code in a way that batches all of your uses and
    /// use the system callers instead of one-by-one use of this function.
    /// Returns nullptr if entity or component does not exist.
    template <typename... ComponentTypes>
    [[nodiscard]] std::tuple<ComponentTypes*...> get_components(const entity_id_t id) const
    {
        const auto entity_search = entities.find(id);
        if (entities.end() == entity_search)
            return {
                reinterpret_cast<ComponentTypes*>(std::uintptr_t { 0 })...,
            };
        const auto& e = entity_search->second;
        return {
            e.archetype->get_component<ComponentTypes>(e.components)...,
        };
    }

    [[nodiscard]] Archetype* get_archetype(const Archetype::id_t&);
    [[nodiscard]] Entity* get_entity(entity_id_t);
    [[nodiscard]] const Entity* get_entity(entity_id_t) const;
    [[nodiscard]] Entity* get_entity(const std::string&);
    [[nodiscard]] const Entity* get_entity(const std::string&) const;

    /// Highly optimized way of system execution
    template <typename Condition, typename F>
    void parallel_system(F&& fun)
    {
        for (auto& archetype : archetypes) {
            if (!archetype.second->satisfy<Condition>()) {
                continue;
            }
            archetype.second->parallel_system_conditioned<Condition>(fun);
        }
    }

    /// Less performant way of system execution.
    /// Good for highly conflicting systems, can not be executed in parallel way.
    template <typename Condition, typename F>
    void synchronised_system(F&& fun)
    {
        for (auto& archetype : archetypes) {
            if (!archetype.second->satisfy<Condition>()) {
                continue;
            }
            archetype.second->synchronised_system_conditioned<Condition>(fun);
        }
    }

    /// It will do all the delayed actions
    void update();

    void show_debug_gui() const;

    void read(const std::shared_ptr<platform::stream::Stream>& stream, job::EndCaller<>&& end_callback);
    void write(const std::shared_ptr<platform::stream::Stream>& stream, job::EndCaller<>&& end_callback) const;
    void resolve(resolver_t&& r);
};
}