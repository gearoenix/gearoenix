#pragma once
#include "../gx-cr-object-type-indices.hpp"
#include "../gx-cr-object.hpp"
#include "../gx-cr-static-flat-map.hpp"
#include "gx-cr-ecs-condition.hpp"
#include "gx-cr-ecs-entity-ptr.hpp"
#include "gx-cr-ecs-types.hpp"

namespace gearoenix::core {
struct ObjectStreamer;
}

namespace gearoenix::core::ecs {
struct World;
struct Archetype;
struct Component;

struct Entity final : Object {
    friend struct World;
    friend struct Archetype;

    constexpr static std::size_t max_components = 32;
    constexpr static auto max_count = 1024 * 16;
    constexpr static auto object_type_index = gearoenix_core_ecs_entity_type_index;

    typedef static_flat_map<object_type_index_t, std::shared_ptr<Component>, max_components> components_t;
    typedef boost::container::flat_map<object_id_t, EntityPtr> children_t;

    GX_GET_PTR_PRV(Entity, parent);
    GX_GET_PTR_PRV(Archetype, archetype);

    std::mutex components_lock;
    GX_GET_CREF_PRV(components_t, all_types_to_components);

    std::mutex children_lock;
    GX_GET_REF_PRV(children_t, children);

    void write(std::shared_ptr<platform::stream::Stream>&&, std::shared_ptr<ObjectStreamer>&&, job::EndCaller<>&&) override;

    template <typename T>
    struct ConditionCheck final {
        [[nodiscard]] constexpr static bool match(const components_t& id)
        {
            return id.contains(T::object_type_index);
        }
    };

    template <typename Condition>
    struct ConditionCheck<Not<Condition>> final {
        [[nodiscard]] constexpr static bool match(const components_t& id)
        {
            return !ConditionCheck<Condition>::match(id);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<All<Conditions...>> final {
        [[nodiscard]] constexpr static bool match(const components_t& id)
        {
            return (ConditionCheck<Conditions>::match(id) && ...);
        }
    };

    template <typename... Conditions>
    struct ConditionCheck<Any<Conditions...>> final {
        [[nodiscard]] constexpr static bool match(const components_t& id)
        {
            return (ConditionCheck<Conditions>::match(id) || ...);
        }
    };

public:
    explicit Entity(std::string&& name);
    [[nodiscard]] static EntityPtr construct(std::string&& name, Entity* parent);
    ~Entity() override;
    void set_parent(Entity* p);
    void add_child(EntityPtr&& child);
    /// It adds itself and all of its children to the World, but it doesn't add its parents.
    void add_to_world();
    void show_debug_gui() override;
    void add_component(std::shared_ptr<Component>&& component);
    void remove_component(object_type_index_t);
    [[nodiscard]] bool contains_in_parents(const Entity* e) const;

    [[nodiscard]] const std::shared_ptr<Component>& get_component(object_type_index_t component_type) const;

    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component() const
    {
        return static_cast<ComponentType*>(get_component(ComponentType::object_type_index).get());
    }

    template <typename ComponentType>
    [[nodiscard]] std::shared_ptr<ComponentType> get_component_shared_ptr() const
    {
        static_assert(std::is_base_of_v<Component, ComponentType>);
        return std::static_pointer_cast<ComponentType>(get_component(ComponentType::object_type_index));
    }

    template <typename Condition>
    [[nodiscard]] constexpr bool satisfy() const
    {
        return ConditionCheck<Condition>::match(all_types_to_components);
    }
};
}
