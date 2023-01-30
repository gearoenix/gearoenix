#ifndef GEAROENIX_CORE_ECS_ENTITY_HPP
#define GEAROENIX_CORE_ECS_ENTITY_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "../sync/gx-cr-sync-end-caller.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <atomic>
#include <boost/container/flat_map.hpp>
#include <optional>
#include <string>
#include <vector>

namespace gearoenix::core::ecs {
struct World;
struct Archetype;
struct EntityBuilder;
struct Entity final {
    friend struct World;
    friend struct EntityBuilder;

private:
    static std::atomic<entity_id_t> last_id;

    Archetype* archetype = nullptr;
    unsigned char* components = nullptr;
    std::string name;

    Entity(Archetype* archetype, unsigned char* components, std::string&& name) noexcept;

public:
    Entity(Entity&&) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) noexcept = default;
    Entity& operator=(const Entity&) = delete;
    void show_debug_gui() noexcept;
};

struct EntityBuilder final {
    friend struct World;
    typedef boost::container::flat_map<std::type_index, std::unique_ptr<Component>> components_t;

    GX_GET_CVAL_PRV(entity_id_t, id);
    GX_GETSET_CREF_PRV(std::string, name);

private:
    components_t components;
    sync::EndCaller end_caller;

    EntityBuilder(entity_id_t, std::string&& name, sync::EndCaller&& end_caller) noexcept;

public:
    EntityBuilder(std::string&& name, sync::EndCaller&& end_caller) noexcept;
    EntityBuilder(EntityBuilder&&) noexcept;
    EntityBuilder(const EntityBuilder&) = delete;
    EntityBuilder& operator=(EntityBuilder&&) = delete;
    EntityBuilder& operator=(const EntityBuilder&) = delete;

    template <typename ComponentType>
    void add_component(ComponentType&& component) noexcept
    {
        Component::types_check<ComponentType>();
        auto c = std::make_pair(
            Component::create_type_index<ComponentType>(),
            std::make_unique<ComponentType>(std::forward<ComponentType>(component)));
        const auto search = components.find(c.first);
        if (search != components.end())
            GX_LOG_F("Component '" << typeid(ComponentType).name() << "' already exists in entity '" << id);
        components.emplace(std::move(c));
    }

    template <typename... ComponentType>
    void add_components(ComponentType&&... cs) noexcept
    {
        ((add_component(std::move(cs))), ...);
    }

    [[nodiscard]] const Component* get_component(std::type_index component_type) const noexcept;
    [[nodiscard]] Component* get_component(std::type_index component_type) noexcept;

    template <typename ComponentType>
    const ComponentType* get_component() const noexcept
    {
        return reinterpret_cast<const ComponentType*>(get_component(std::type_index(typeid(ComponentType))));
    }

    template <typename ComponentType>
    ComponentType* get_component() noexcept
    {
        return reinterpret_cast<ComponentType*>(get_component(std::type_index(typeid(ComponentType))));
    }
};

struct EntitySharedBuilder final {
    friend struct World;
    GX_GET_REF_PRV(EntityBuilder, builder);

private:
    World* const world;

    EntitySharedBuilder(World* world, std::string&& name, sync::EndCaller&& end_caller) noexcept;

public:
    ~EntitySharedBuilder() noexcept;
    [[nodiscard]] entity_id_t get_id() const noexcept { return builder.get_id(); }
};
}

#endif