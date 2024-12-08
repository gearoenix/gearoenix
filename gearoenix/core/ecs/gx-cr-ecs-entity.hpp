#pragma once
#include "../job/gx-cr-job-end-caller.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-cr-ecs-types.hpp"
#include <atomic>
#include <memory>
#include <string>

namespace gearoenix::platform::stream {
struct Stream;
}

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
    void show_debug_gui() const;
    void write(std::shared_ptr<platform::stream::Stream>&& stream, job::EndCaller<>&& end_callback) const;
};
}