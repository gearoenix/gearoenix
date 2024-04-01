#ifndef GEAROENIX_CORE_ECS_ENTITY_HPP
#define GEAROENIX_CORE_ECS_ENTITY_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../job/gx-cr-job-end-caller.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <atomic>
#include <boost/container/flat_map.hpp>
#include <string>

namespace gearoenix::core::ecs {
struct World;
struct Archetype;
struct EntityBuilder;
struct Component;

struct Entity final {
    friend struct World;
    friend struct EntityBuilder;

private:
    static std::atomic<entity_id_t> last_id;

    Archetype* archetype = nullptr;
    std::shared_ptr<Component>* components = nullptr;
    std::string name;

    Entity(Archetype* archetype, std::shared_ptr<Component>* components, std::string&& name) noexcept;

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    Entity& operator=(const Entity&) = delete;
    void show_debug_gui();
};

struct EntityBuilder final {
    friend struct World;
    friend struct EntitySharedBuilder;

    /// Only final types are used in the key
    typedef boost::container::flat_map<std::type_index, std::shared_ptr<Component>> components_t;
    typedef boost::container::flat_map<std::type_index, std::type_index> bases_to_leaves_t;

    GX_GET_CVAL_PRV(entity_id_t, id);
    GX_GETSET_CREF_PRV(std::string, name);

    components_t components;
    bases_to_leaves_t bases_to_leaves;
    job::EndCaller<> end_caller;

    EntityBuilder(entity_id_t, std::string&& name, job::EndCaller<void>&& end_caller) noexcept;

public:
    EntityBuilder(std::string&& name, job::EndCaller<void>&& end_caller) noexcept;
    EntityBuilder(EntityBuilder&&) noexcept;
    EntityBuilder(const EntityBuilder&) = delete;
    EntityBuilder& operator=(EntityBuilder&&) = delete;
    EntityBuilder& operator=(const EntityBuilder&) = delete;

    void add_component(std::shared_ptr<Component>&& component);

    template <typename... ComponentType>
    void add_components(std::shared_ptr<ComponentType>&&... cs)
    {
        ((add_component(std::move(cs))), ...);
    }

    [[nodiscard]] const std::shared_ptr<Component>& get_component(std::type_index component_type) const;
    [[nodiscard]] const std::shared_ptr<Component>& get_component_final_type(std::type_index component_type) const;

    template <typename ComponentType>
    ComponentType* get_component() const
    {
        if constexpr (std::is_final_v<ComponentType>) {
            return static_cast<ComponentType*>(get_component_final_type(std::type_index(typeid(ComponentType))).get());
        } else {
            return dynamic_cast<ComponentType*>(get_component(std::type_index(typeid(ComponentType))).get());
        }
    }
};

struct EntitySharedBuilder final {
    friend struct World;
    GX_GET_REF_PRV(EntityBuilder, builder);

    World* const world;

    EntitySharedBuilder(World* world, std::string&& name, job::EndCaller<>&& end_caller);

public:
    ~EntitySharedBuilder();
    [[nodiscard]] entity_id_t get_id() const { return builder.get_id(); }
};
}

#endif