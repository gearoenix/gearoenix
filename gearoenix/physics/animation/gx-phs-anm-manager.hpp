#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>
#include <mutex>
#include <string>

namespace gearoenix::render::material {
struct Sprite;
}

namespace gearoenix::physics::animation {
struct Armature;
struct Animation;
struct Bone;
struct Manager final : core::Singleton<Manager> {
private:
    std::mutex this_lock;
    boost::container::flat_map<std::string, std::shared_ptr<Animation>> animations;

public:
    Manager();
    ~Manager() override;

    [[nodiscard]] core::ecs::EntityPtr create_armature(
        std::string&& name,
        core::ecs::Entity* parent,
        std::shared_ptr<Bone>&& root_bone);
    void create_sprite(
        core::ecs::EntityPtr& entity,
        std::shared_ptr<render::material::Sprite>&& sprite,
        std::uint32_t width,
        std::uint32_t height);
    void update() const;
};
}