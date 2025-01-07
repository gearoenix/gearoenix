#pragma once
#include "../job/gx-cr-job-end-caller.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-types.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core::ecs {
struct Archetype;
struct Component;
struct EntityBuilder;
struct World;
struct EntityBuilder final {
    friend struct World;
    friend struct EntitySharedBuilder;

    typedef boost::container::flat_map<component_index_t, std::shared_ptr<Component>> components_t;

    GX_GET_CVAL_PRV(entity_id_t, id);
    GX_GETSET_CREF_PRV(std::string, name);
    GX_GET_CREF_PRV(component_index_set_t, final_types);
    GX_GET_CREF_PRV(component_index_set_t, all_types);
    components_t all_types_to_components;
    job::EndCaller<> end_caller;
    std::mutex components_lock; // for add and deletion

    EntityBuilder(entity_id_t, std::string&& name, job::EndCaller<>&& end_caller);

public:
    EntityBuilder(std::string&& name, job::EndCaller<>&& end_caller);
    EntityBuilder(EntityBuilder&&) noexcept;
    EntityBuilder(const EntityBuilder&) = delete;
    EntityBuilder& operator=(EntityBuilder&&) = delete;
    EntityBuilder& operator=(const EntityBuilder&) = delete;
    ~EntityBuilder();

    static void construct(
        entity_id_t id,
        std::shared_ptr<platform::stream::Stream>&& stream,
        job::EndCallerShared<EntityBuilder>&& end_caller);

    void add_component(std::shared_ptr<Component>&& component);

    template <typename... ComponentType>
    void add_components(std::shared_ptr<ComponentType>&&... cs)
    {
        ((add_component(std::move(cs))), ...);
    }

    [[nodiscard]] const std::shared_ptr<Component>& get_component(component_index_t component_type) const;

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
    GX_GET_REF_PRV(EntityBuilder, builder);

public:
    EntitySharedBuilder(std::string&& name, job::EndCaller<>&& end_caller);
    ~EntitySharedBuilder();
    [[nodiscard]] entity_id_t get_id() const { return builder.get_id(); }
    [[nodiscard]] const std::string& get_name() const { return builder.get_name(); }
};
}