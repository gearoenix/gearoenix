#pragma once
#include "../gx-cr-singleton.hpp"
#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-entity-ptr.hpp"
#include <memory>
#include <variant>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core::ecs {
typedef boost::container::static_vector<std::shared_ptr<Component>, Entity::max_components> components_vec_t;

struct Archetype;

/// The World of ECS
///
/// Main focus of this struct is the performance of systems and memory usage
struct World final : Singleton<World> {
    typedef std::function<bool()> resolver_t;

    constexpr static auto max_archetypes_count = 16;

private:
    static_flat_map<component_index_set_t, std::unique_ptr<Archetype>, max_archetypes_count> archetypes;

    std::mutex resolvers_lock;
    std::vector<resolver_t> resolvers;

    struct Action final {
        struct Add final {
            Entity* entity;
        };

        struct Delete final {
            std::shared_ptr<Entity> entity;
        };

        struct PullOut final {
            job::EndCaller<EntityPtr> end_caller;
        };

        std::variant<Add, Delete, PullOut> variant;
    };

    std::mutex delayed_actions_lock;
    std::vector<Action> delayed_actions;

public:
    World();
    ~World() override;
    World(World&&) = delete;
    World(const World&) = delete;

    /// You must know your context (state of the world), unless you want to end up having race
    void add_entity(Entity*);
    /// Recommended way to add an entity, in case you do not know the context you're in.
    void delayed_add_entity(Entity*);

    void delete_entity(std::shared_ptr<Entity>&&);
    void delayed_delete_entity(std::shared_ptr<Entity>&&);

    void pullout_entity(job::EndCaller<EntityPtr>&&);
    void delayed_pullout_entity(EntityPtr&&, job::EndCaller<EntityPtr>&&);

    [[nodiscard]] Archetype* get_archetype(const EntityPtr&);
    [[nodiscard]] Archetype* get_archetype(const Entity*);

    /// Highly optimized way of system execution
    template <typename Condition, typename F>
    void parallel_system(const F& fun)
    {
        for (const auto& archetype : archetypes) {
            auto& archetype_ref = *archetype.second;
            if (!archetype_ref.satisfy<Condition>()) {
                continue;
            }
            archetype_ref.parallel_system_conditioned<Condition>(fun);
        }
    }

    /// Less performant way of system execution.
    /// Good for highly conflicting systems, it can't be executed parallelly.
    template <typename Condition, typename F>
    void synchronised_system(const F& fun)
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
    void resolve(resolver_t&& r);
    void clear();
};
}
