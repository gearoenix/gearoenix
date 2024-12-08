#pragma once
#include "gx-cr-ecs-comp-allocator.hpp"
#include "gx-cr-ecs-component.hpp"
#include "gx-cr-ecs-entity-builder.hpp"
#include "gx-cr-ecs-world.hpp"

namespace gearoenix::core::ecs {
struct Singleton final {
private:
    static boost::container::flat_map<component_index_t, std::shared_ptr<Component>> values;

public:
    template <typename T>
    static T& get()
    {
        constexpr auto ti = ComponentType::create_index<T>();
        static_assert(T::MAX_COUNT == 1, "Only types with 1 max count are counted as singleton.");
        auto& value = values[ti];
        if (value) {
            return *static_cast<T*>(value.get());
        }
        {
            Component* search = nullptr;
            World::get()->synchronised_system<T>([&](const auto, auto* const ptr) {
                GX_ASSERT_D(nullptr == search); // only one must exist
                search = ptr;
            });

            if (nullptr != search) {
                value = search->get_component_self().lock();
                return *static_cast<T*>(value.get());
            }
        }

        EntityBuilder builder(ComponentType::get_name(ti) + "-singleton-entity", job::EndCaller([] { }));
        if (nullptr == value) {
            value = construct_component<T>(builder.get_id());
        } else {
            value->set_entity_id(builder.get_id());
        }
        builder.add_component(std::shared_ptr(value));
        World::get()->delayed_create_entity(std::move(builder));
        return *static_cast<T*>(value.get());
    }
};
}