#include "gx-phs-anm-bone.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../gx-phs-transformation.hpp"
#include <vector>

void gearoenix::physics::animation::Bone::write_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<>&&) const
{
    s->write_fail_debug(static_cast<std::uint32_t>(scale_samples.size()));
    for (const auto& [t, key] : scale_samples) {
        s->write_fail_debug(t);
        key.write(*s);
    }
    s->write_fail_debug(static_cast<std::uint32_t>(rotation_samples.size()));
    for (const auto& [t, key] : rotation_samples) {
        s->write_fail_debug(t);
        key.write(*s);
    }
    s->write_fail_debug(static_cast<std::uint32_t>(translation_samples.size()));
    for (const auto& [t, key] : translation_samples) {
        s->write_fail_debug(t);
        key.write(*s);
    }
    s->write_fail_debug(static_cast<std::uint32_t>(children.size()));
    for (const auto& child : children) {
        s->write_fail_debug(child->entity_id);
    }
    s->write_fail_debug(parent ? parent->entity_id : core::ecs::invalid_entity_id);
}

void gearoenix::physics::animation::Bone::update_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<>&& end)
{
    const auto scale_samples_count = s->read<std::uint32_t>();
    scale_samples.clear();
    scale_samples.reserve(scale_samples_count);
    for (auto i = decltype(scale_samples_count) { 0 }; i < scale_samples_count; ++i) {
        const auto t = s->read<double>();
        Keyframe<math::Vec3<double>> key;
        key.read(*s);
        scale_samples.emplace(t, key);
    }

    const auto rotation_samples_count = s->read<std::uint32_t>();
    rotation_samples.clear();
    rotation_samples.reserve(rotation_samples_count);
    for (auto i = decltype(rotation_samples_count) { 0 }; i < rotation_samples_count; ++i) {
        const auto t = s->read<double>();
        Keyframe<math::Quat<double>> key;
        key.read(*s);
        rotation_samples.emplace(t, key);
    }

    const auto translation_samples_count = s->read<std::uint32_t>();
    translation_samples.clear();
    translation_samples.reserve(translation_samples_count);
    for (auto i = decltype(translation_samples_count) { 0 }; i < translation_samples_count; ++i) {
        const auto t = s->read<double>();
        Keyframe<math::Vec3<double>> key;
        key.read(*s);
        translation_samples.emplace(t, key);
    }

    std::vector<core::ecs::entity_id_t> children_ids(s->read<std::uint32_t>());
    for (auto& child_id : children_ids) {
        s->read(child_id);
    }
    const auto parent_id = s->read<core::ecs::entity_id_t>();

    parent = nullptr;
    children_ids.clear();
    transform = nullptr;

    core::ecs::World::get()->resolve([this, self = component_self.lock(), children_ids = std::move(children_ids), parent_id, end]() mutable -> bool {
        (void)self;
        (void)end;
        bool not_resolved = false;
        if (transform == nullptr) {
            transform = core::ecs::World::get()->get_component_shared_ptr<Transformation>(entity_id);
            not_resolved = transform == nullptr;
        }
        if (parent_id != core::ecs::invalid_entity_id && parent == nullptr) {
            parent = core::ecs::World::get()->get_component<Bone>(parent_id);
            not_resolved = parent == nullptr;
        }
        for (const auto& child_id : children_ids) {
            GX_ASSERT_D(child_id != core::ecs::invalid_entity_id);
            if (auto* const child = core::ecs::World::get()->get_component<Bone>(child_id); child != nullptr) {
                add_child(child);
            } else {
                not_resolved = true;
            }
        }
        return not_resolved;
    });
}

gearoenix::physics::animation::Bone::Bone(
    Bone* const parent,
    std::shared_ptr<Transformation>&& transform,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , transform(std::move(transform))
    , parent(parent)
{
    if (parent) {
        parent->add_child(this);
    }
}

gearoenix::physics::animation::Bone::~Bone()
{
    if (parent) {
        parent->children.erase(this);
    }
}

void gearoenix::physics::animation::Bone::add_child(Bone* const child)
{
    GX_ASSERT_D(child);
    child->parent = this;
    children.insert(child);
}
