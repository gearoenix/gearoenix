#include "gx-phs-anm-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-phs-transformation.hpp"
#include "gx-phs-anm-animation.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"

namespace {
gearoenix::physics::animation::Manager* instance;
}

gearoenix::physics::animation::Manager::Manager()
{
    core::ecs::ComponentType::add<AnimationPlayer>();
    core::ecs::ComponentType::add<Armature>();
    core::ecs::ComponentType::add<Bone>();
    GX_ASSERT_D(!instance);
    instance = this;
}

gearoenix::physics::animation::Manager::~Manager()
{
    GX_ASSERT_D(instance);
    instance = nullptr;
    animations.clear();
}

gearoenix::physics::animation::Manager* gearoenix::physics::animation::Manager::get()
{
    return instance;
}

gearoenix::core::ecs::EntityPtr gearoenix::physics::animation::Manager::create_armature(
    std::string&& name,
    core::ecs::Entity* const parent,
    std::shared_ptr<Bone>&& root_bone)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);

    auto transform = core::Object::construct<Transformation>(entity->get_object_name() + "-transformation");

    auto anim = core::Object::construct<ArmatureAnimation>(entity->get_object_name() + "-animation", std::shared_ptr(root_bone));

    auto armature = core::Object::construct<Armature>(entity->get_object_name() + "-armature", transform.get());
    armature->set_root_bone(std::move(root_bone));

    entity->add_component(std::move(armature));
    entity->add_component(std::move(transform));

    entity->add_component(core::Object::construct<AnimationPlayer>(std::shared_ptr(anim), entity->get_object_name() + "-animation-player", 0.0));
    auto anim_name = anim->get_object_name();
    const std::lock_guard _lg(this_lock);
    animations.emplace(std::move(anim_name), std::move(anim));

    return entity;
}

void gearoenix::physics::animation::Manager::create_sprite(
    core::ecs::EntityPtr& entity,
    std::shared_ptr<render::material::Sprite>&& sprite,
    const std::uint32_t width,
    const std::uint32_t height)
{
    auto anim = core::Object::construct<SpriteAnimation>(
        entity->get_object_name() + "-sprite-animation", std::move(sprite), width, height);
    auto player = core::Object::construct<AnimationPlayer>(
        std::shared_ptr(anim), entity->get_object_name() + "-animation-player", 0.0);
    player->set_loop_range_time(0.0, 0.999);
    entity->add_component(std::move(player));
    auto anim_name = anim->get_object_name();
    const std::lock_guard _lg(this_lock);
    animations.emplace(std::move(anim_name), std::move(anim));
}

void gearoenix::physics::animation::Manager::update() const
{
    core::ecs::World::get().parallel_system<AnimationPlayer>([this](auto, auto* const player, auto) {
        player->update_time(render::engine::Engine::get().get_delta_time());
        player->animate();
    });
}
