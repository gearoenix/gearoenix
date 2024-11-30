#pragma once
#include "../job/gx-cr-job-end-caller.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <atomic>
#include <memory>

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
    GX_GET_CREF_PRV(std::string, name);

    Entity(Archetype* archetype, std::shared_ptr<Component>* components, std::string&& name);

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    Entity& operator=(const Entity&) = delete;
    ~Entity();
    void show_debug_gui(const render::engine::Engine&) const;
};

struct EntityBuilder final {
    friend struct World;
    friend struct EntitySharedBuilder;

    typedef boost::container::flat_map<Component::TypeIndex, std::shared_ptr<Component>> components_t;

    GX_GET_CVAL_PRV(entity_id_t, id);
    GX_GETSET_CREF_PRV(std::string, name);
    GX_GET_CREF_PRV(Component::TypeIndexSet, final_types);
    GX_GET_CREF_PRV(Component::TypeIndexSet, all_types);
    components_t all_types_to_components;
    job::EndCaller<> end_caller;

    EntityBuilder(entity_id_t, std::string&& name, job::EndCaller<>&& end_caller);

public:
    EntityBuilder(std::string&& name, job::EndCaller<>&& end_caller);
    EntityBuilder(EntityBuilder&&) noexcept;
    EntityBuilder(const EntityBuilder&) = delete;
    EntityBuilder& operator=(EntityBuilder&&) = delete;
    EntityBuilder& operator=(const EntityBuilder&) = delete;
    ~EntityBuilder();

    void add_component(std::shared_ptr<Component>&& component);

    template <typename... ComponentType>
    void add_components(std::shared_ptr<ComponentType>&&... cs)
    {
        ((add_component(std::move(cs))), ...);
    }

    [[nodiscard]] const std::shared_ptr<Component>& get_component(Component::TypeIndex component_type) const;

    template <typename ComponentType>
    [[nodiscard]] ComponentType* get_component() const
    {
        return static_cast<ComponentType*>(get_component(ComponentType::TYPE_INDEX).get());
    }

    template <typename ComponentType>
    [[nodiscard]] std::shared_ptr<ComponentType> get_component_shared_ptr() const
    {
        static_assert(std::is_base_of_v<Component, ComponentType>);
        return std::static_pointer_cast<ComponentType>(get_component(ComponentType::TYPE_INDEX));
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