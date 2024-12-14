#pragma once
#include "../job/gx-cr-job-end-caller.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-types.hpp"
#include <memory>
#include <string>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core::ecs {
struct Component {
    friend struct Singleton;
    friend struct ComponentType;

    template <typename T, typename... Args>
    friend std::shared_ptr<T> construct_component(Args&&... args);

    GX_GET_REFC_PRT(component_index_t, final_type_index);
    GX_GET_REFC_PRT(std::string, name);
    GX_GETSET_VAL_PRT(bool, enabled, true);
    GX_GET_CREF_PRT(std::weak_ptr<Component>, component_self);
    GX_GETSET_VAL_PRT(entity_id_t, entity_id, invalid_entity_id);

    virtual void write_in_io_context(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end_callback) const;
    virtual void update_in_io_context(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end_callback);

public:
    Component(component_index_t final_type_index, std::string&& name, entity_id_t entity_id);
    virtual ~Component() = default;
    Component(Component&&) = delete;
    Component(const Component&) = delete;
    Component& operator=(Component&&) = delete;
    Component& operator=(const Component&) = delete;

    virtual void show_debug_gui();
    void write(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end_callback) const;
    void update(std::shared_ptr<platform::stream::Stream>&& s, job::EndCaller<>&& e);
    static void read(std::shared_ptr<platform::stream::Stream>&& s, job::EndCallerShared<Component>&& e);
    [[nodiscard]] static component_index_t read_final_type_index(platform::stream::Stream&);
};
}