#include "gx-phs-anm-bone.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../gx-phs-transformation.hpp"

void gearoenix::physics::animation::Bone::write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end_caller)
{
    Component::write(std::shared_ptr(stream), std::move(object_streamer), std::move(end_caller));
    stream->write_fail_debug(static_cast<std::uint32_t>(scale_samples.size()));
    for (const auto& [t, key] : scale_samples) {
        stream->write_fail_debug(t);
        key.write(*stream);
    }
    stream->write_fail_debug(static_cast<std::uint32_t>(rotation_samples.size()));
    for (const auto& [t, key] : rotation_samples) {
        stream->write_fail_debug(t);
        key.write(*stream);
    }
    stream->write_fail_debug(static_cast<std::uint32_t>(translation_samples.size()));
    for (const auto& [t, key] : translation_samples) {
        stream->write_fail_debug(t);
        key.write(*stream);
    }
}

void gearoenix::physics::animation::Bone::read(platform::stream::Stream& s, core::job::EndCaller<>&& end)
{
    const auto scale_samples_count = s.read<std::uint32_t>();
    scale_samples.clear();
    scale_samples.reserve(scale_samples_count);
    for (auto i = decltype(scale_samples_count) { 0 }; i < scale_samples_count; ++i) {
        const auto t = s.read<double>();
        Keyframe<math::Vec3<double>> key;
        key.read(s);
        scale_samples.emplace(t, key);
    }

    const auto rotation_samples_count = s.read<std::uint32_t>();
    rotation_samples.clear();
    rotation_samples.reserve(rotation_samples_count);
    for (auto i = decltype(rotation_samples_count) { 0 }; i < rotation_samples_count; ++i) {
        const auto t = s.read<double>();
        Keyframe<math::Quat<double>> key;
        key.read(s);
        rotation_samples.emplace(t, key);
    }

    const auto translation_samples_count = s.read<std::uint32_t>();
    translation_samples.clear();
    translation_samples.reserve(translation_samples_count);
    for (auto i = decltype(translation_samples_count) { 0 }; i < translation_samples_count; ++i) {
        const auto t = s.read<double>();
        Keyframe<math::Vec3<double>> key;
        key.read(s);
        translation_samples.emplace(t, key);
    }

    transform = nullptr;

    core::ecs::World::get().resolve([this, self = object_self.lock(), end = std::move(end)]() mutable -> bool {
        (void)self;
        (void)end;
        bool not_resolved = false;
        GX_TODO; // object streamer can have a better solution for this, instead of resolving
        if (transform == nullptr) {
            transform = entity->get_component_shared_ptr<Transformation>();
            not_resolved |= transform == nullptr;
        }
        return not_resolved;
    });
}

gearoenix::physics::animation::Bone::Bone(core::ecs::Entity* const entity, std::shared_ptr<Transformation>&& transform, std::string&& name)
    : Component(entity, core::ecs::ComponentType::create_index(this), std::move(name))
    , transform(std::move(transform))
{
}

gearoenix::physics::animation::Bone::~Bone() = default;

gearoenix::core::ecs::EntityPtr gearoenix::physics::animation::Bone::build(std::string&& name, core::ecs::Entity* const parent)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    auto transform = Object::construct<Transformation>(entity.get(), entity->get_object_name() + "-transformation");
    entity->add_component(std::shared_ptr(transform));
    entity->add_component(Object::construct<Bone>(entity.get(), std::move(transform), entity->get_object_name() + "-bone"));
    return entity;
}

void gearoenix::physics::animation::Bone::set_inverse_bind_matrix(const math::Mat4x4<double>& m)
{
    inverse_bind_matrix = m;
    bind_matrix = m.inverted().transposed();
}

void gearoenix::physics::animation::Bone::update_matrices()
{
    global_matrix = transform->get_global_matrix() * inverse_bind_matrix;
    transposed_inverted_global_matrix = transform->get_transposed_inverted_global_matrix() * bind_matrix;
}

void gearoenix::physics::animation::Bone::update_all_bones_after_transform_updates()
{
    core::ecs::World::get().parallel_system<Bone>([](auto, auto* const bone, auto) {
        bone->update_matrices();
    });
}
