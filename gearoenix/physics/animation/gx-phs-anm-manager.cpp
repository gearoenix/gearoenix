#include "gx-phs-anm-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-phs-transformation.hpp"
#include "gx-phs-anm-animation.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"

namespace {
gearoenix::physics::animation::Manager* manager_ptr;
}

gearoenix::physics::animation::Manager::Manager()
{
    core::ecs::ComponentType::add<AnimationPlayer>();
    core::ecs::ComponentType::add<Armature>();
    core::ecs::ComponentType::add<Bone>();
    GX_ASSERT_D(!manager_ptr);
    manager_ptr = this;
}

gearoenix::physics::animation::Manager::~Manager()
{
    GX_ASSERT_D(manager_ptr);
    manager_ptr = nullptr;
    animations_map.clear();
}

gearoenix::physics::animation::Manager* gearoenix::physics::animation::Manager::get()
{
    return manager_ptr;
}

std::shared_ptr<gearoenix::core::ecs::EntitySharedBuilder> gearoenix::physics::animation::Manager::create_armature_builder(
    std::string&& name,
    Transformation* const parent_transform,
    std::shared_ptr<Bone>&& root_bone,
    core::job::EndCaller<>&& entity_end_callback)
{
    auto eb = std::make_shared<core::ecs::EntitySharedBuilder>(std::move(name), std::move(entity_end_callback));
    auto transform = core::ecs::construct_component<Transformation>(
        eb->get_name() + "-transformation", parent_transform, eb->get_id());
    auto& b = eb->get_builder();
    auto anim = std::make_shared<ArmatureAnimation>(b.get_name() + "-animation", std::shared_ptr(root_bone));
    b.add_component(std::shared_ptr(transform));
    auto armature = core::ecs::construct_component<Armature>(eb->get_name() + "-armature", std::move(transform), eb->get_id());
    armature->set_root_bone(std::move(root_bone));
    b.add_component(std::move(armature));
    b.add_component(core::ecs::construct_component<AnimationPlayer>(std::shared_ptr(anim), b.get_name() + "-animation-player", 0.0, b.get_id()));
    auto anim_name = anim->name;
    const std::lock_guard _lg(this_lock);
    animations_map.emplace(std::move(anim_name), std::move(anim));
    return eb;
}

void gearoenix::physics::animation::Manager::create_sprite(
    core::ecs::EntityBuilder& builder,
    std::shared_ptr<render::material::Sprite>&& sprite,
    const std::uint32_t width,
    const std::uint32_t height)
{
    auto anim = std::make_shared<SpriteAnimation>(
        builder.get_name() + "-sprite-animation", std::move(sprite), width, height);
    auto player = core::ecs::construct_component<AnimationPlayer>(
        std::shared_ptr(anim), builder.get_name() + "-animation-player", 0.0, builder.get_id());
    player->set_loop_range_time(0.0, 0.999);
    builder.add_component(std::move(player));
    auto anim_name = anim->name;
    const std::lock_guard _lg(this_lock);
    animations_map.emplace(std::move(anim_name), std::move(anim));
}

void gearoenix::physics::animation::Manager::update() const
{
    core::ecs::World::get()->parallel_system<AnimationPlayer>([this](auto, AnimationPlayer* const player, auto) {
        player->update_time(render::engine::Engine::get()->get_delta_time());
        player->animate();
    });
}
