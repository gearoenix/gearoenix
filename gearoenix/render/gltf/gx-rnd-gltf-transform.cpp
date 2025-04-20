#include "gx-rnd-gltf-transform.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../scene/gx-rnd-scn-scene.hpp"
#include "gx-rnd-gltf-context.hpp"

void gearoenix::render::gltf::apply_transform(const int node_index, const Context& context, physics::Transformation& transform)
{
    const auto& node = context.data.nodes[node_index];
    const std::vector<double>& rotation = node.rotation;
    GX_ASSERT_D(rotation.empty() || rotation.size() == 4);
    const std::vector<double>& scale = node.scale;
    GX_ASSERT_D(scale.empty() || scale.size() == 3);
    const std::vector<double>& translation = node.translation;
    GX_ASSERT_D(translation.empty() || translation.size() == 3);
    if (scale.size() == 3) {
        transform.local_inner_scale({ scale[0], scale[1], scale[2] });
    }
    if (rotation.size() == 4) {
        transform.local_inner_rotate(math::Quat(rotation[0], rotation[1], rotation[2], rotation[3]));
    }
    if (translation.size() == 3) {
        transform.set_local_position(math::Vec3(translation[0], translation[1], translation[2]));
    }
}

bool gearoenix::render::gltf::has_transformation(const int node_index, const Context& context)
{
    const auto& node = context.data.nodes[node_index];
    return !node.rotation.empty() || !node.scale.empty() || !node.translation.empty();
}

gearoenix::core::ecs::EntityPtr gearoenix::render::gltf::create_empty_entity_transform(
    const int node_index,
    const Context& context,
    core::ecs::Entity* const parent)
{
    const auto& node = context.data.nodes[node_index];
    GX_ASSERT_D(!node.name.empty());
    auto entity = core::ecs::Entity::construct(std::string(node.name), parent);
    auto transform = core::Object::construct<physics::Transformation>(node.name + "-transformation");
    apply_transform(node_index, context, *transform);
    entity->add_component(std::move(transform));
    return entity;
}
