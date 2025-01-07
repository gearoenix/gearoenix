#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace gearoenix::core::ecs {
struct EntityBuilder;
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::material {
struct Sprite;
}

namespace gearoenix::physics::animation {
struct Armature;
struct Animation;
struct Bone;
struct Manager final {
private:
    std::mutex this_lock;
    std::map<std::string, std::shared_ptr<Animation>> animations_map;

public:
    Manager();
    ~Manager();
    [[nodiscard]] static Manager* get();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    void operator=(Manager&& o) = delete;
    void operator=(const Manager& o) = delete;

    [[nodiscard]] std::shared_ptr<core::ecs::EntitySharedBuilder> create_armature_builder(
        std::string&& name,
        Transformation* parent_transform,
        std::shared_ptr<Bone>&& root_bone,
        core::job::EndCaller<>&& entity_end_callback);
    void create_sprite(
        core::ecs::EntityBuilder& builder,
        std::shared_ptr<render::material::Sprite>&& sprite,
        std::uint32_t width,
        std::uint32_t height);

    void update() const;
};
}