#include "gx-rnd-gltf-transform.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../scene/gx-rnd-scn-scene.hpp"
#include "gx-rnd-gltf-context.hpp"

void gearoenix::render::gltf::apply_transform(const int node_index, const Context& context, physics::Transformation& transform)
{
    const auto& node = context.data.nodes[node_index];
    const auto& rotation = node.rotation;
    GX_ASSERT_D(rotation.empty() || rotation.size() == 4);
    const auto& scale = node.scale;
    GX_ASSERT_D(scale.empty() || scale.size() == 3);
    const auto& translation = node.translation;
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

    if (!node.matrix.empty()) {
        const auto local_matrix = math::Mat4x4(
            node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
            node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
            node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
            node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]);

        if (!transform.get_local_matrix().equal(local_matrix)) {
            GX_ASSERT_D(transform.get_local_matrix().equal({})); // The glTF implementation is sending unexpected transform data

            transform.set_local_matrix(local_matrix);
        }
    }
}

bool gearoenix::render::gltf::has_transformation(const int node_index, const Context& context)
{
    const auto& node = context.data.nodes[node_index];
    return !node.rotation.empty() || !node.scale.empty() || !node.translation.empty();
}

gearoenix::core::ecs::EntityPtr gearoenix::render::gltf::create_empty_entity_transform(
    const int node_index, const Context& context, core::ecs::Entity* const parent)
{
    const auto& node = context.data.nodes[node_index];
    GX_ASSERT_D(!node.name.empty());
    auto entity = core::ecs::Entity::construct(std::string(node.name), parent);
    auto transform = core::Object::construct<physics::Transformation>(node.name + "-transformation");
    apply_transform(node_index, context, *transform);
    entity->add_component(std::move(transform));
    return entity;
}
