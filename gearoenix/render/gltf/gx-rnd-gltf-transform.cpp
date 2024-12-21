#include "gx-rnd-gltf-transform.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../../core/ecs/gx-cr-ecs-entity-builder.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
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

gearoenix::physics::Transformation* gearoenix::render::gltf::create_empty_entity_transform(
    const int node_index,
    const Context& context,
    physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder)
{

    if (!has_transformation(node_index, context)) {
        return parent_transform;
    }
    const auto& node = context.data.nodes[node_index];
    GX_ASSERT_D(!node.name.empty());
    const auto entity_builder = std::make_shared<core::ecs::EntitySharedBuilder>(
        std::string(node.name),
        core::job::EndCaller(entity_end_callback));
    const auto transform = core::ecs::construct_component<physics::Transformation>(
        node.name + "-transformation", parent_transform, entity_builder->get_id());
    entity_builder->get_builder().add_component(transform);
    scene_builder->get_scene().add_empty(entity_builder->get_id());
    apply_transform(node_index, context, *transform);
    return transform.get();
}
