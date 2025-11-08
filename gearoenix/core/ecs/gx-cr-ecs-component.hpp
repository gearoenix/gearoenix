#pragma once
#include "../gx-cr-object.hpp"
#include "gx-cr-ecs-types.hpp"

namespace gearoenix::core::ecs {
struct Entity;
struct Component : Object {
    friend struct ComponentType;

    GX_GETSET_VAL_PRT(bool, enabled, true);
    GX_GET_PTR_PRT(Entity, entity);

    Component(Entity* entity, object_type_index_t final_type_index, std::string&& name);
    Component(Entity* entity, object_type_index_t final_type_index, object_id_t object_id, std::string&& name);

    void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer, job::EndCaller<>&& end_caller) override;
    static void read(std::shared_ptr<Component>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer, job::EndCaller<>&& end);

public:
    ~Component() override;
    Component(Component&&) = delete;
    Component(const Component&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(const Component&) = delete;
    void show_debug_gui() override;
    virtual void set_entity(Entity* e);
};
}