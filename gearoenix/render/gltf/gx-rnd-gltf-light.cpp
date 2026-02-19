#include "gx-rnd-gltf-light.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-light.hpp"
#include "../light/gx-rnd-lt-manager.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

gearoenix::render::gltf::Lights::Lights(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Lights::~Lights() = default;

bool gearoenix::render::gltf::Lights::is_light(const int node_index) const
{
    if (const auto& node = context.data.nodes[node_index]; node.light != -1) {
        GX_ASSERT_D(node.children.empty());
        GX_ASSERT_D(node.camera == -1);
        GX_ASSERT_D(node.emitter == -1);
        GX_ASSERT_D(node.mesh == -1);
        GX_ASSERT_D(node.skin == -1);
        return true;
    }
    return false;
}

bool gearoenix::render::gltf::Lights::process(const int node_index, core::ecs::Entity* const parent, const core::job::EndCaller<>& end_callback) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_light(node_index)) {
        return false;
    }
    const auto& light = context.data.lights[node.light];
    const auto& colour_vec = light.color;
    GX_ASSERT_D(3 == colour_vec.size());
    const auto intensity = static_cast<float>(light.intensity / 683.0);
    auto colour = math::Vec3(static_cast<float>(colour_vec[0]), static_cast<float>(colour_vec[1]), static_cast<float>(colour_vec[2]));
    GX_ASSERT_D(0.0f <= colour.x);
    GX_ASSERT_D(1.0f >= colour.x);
    GX_ASSERT_D(0.0f <= colour.y);
    GX_ASSERT_D(1.0f >= colour.y);
    GX_ASSERT_D(0.0f <= colour.z);
    GX_ASSERT_D(1.0f >= colour.z);
    colour *= intensity;
    GX_ASSERT_D(light.type == "directional"); // We don't have support for other types yet
    light::Manager::get().build_shadow_caster_directional(std::string(node.name), parent, 1024, 20.0f, 1.0f, 1.0f, core::job::EndCaller<core::ecs::EntityPtr>([this, node_index, e = end_callback, colour](auto&& l) {
        l->template get_component<light::Light>()->colour = colour;
        apply_transform(node_index, context, *l->template get_component<light::ShadowCasterDirectional>()->get_shadow_transform());
        (void)e;
    }));
    return true;
}
