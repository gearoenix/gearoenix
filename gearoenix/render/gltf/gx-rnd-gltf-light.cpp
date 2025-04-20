#include "gx-rnd-gltf-light.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-light.hpp"
#include "../light/gx-rnd-lt-manager.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

namespace {
constexpr auto light_ext_name = "KHR_lights_punctual";
}

gearoenix::render::gltf::Lights::Lights(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Lights::~Lights() = default;

bool gearoenix::render::gltf::Lights::is_light(const int node_index) const
{
    if (const auto& node = context.data.nodes[node_index]; node.light != -1 || (node.extensions.size() == 1 && node.extensions.begin()->first == light_ext_name)) {
        GX_ASSERT_D(node.children.empty());
        GX_ASSERT_D(node.camera == -1);
        GX_ASSERT_D(node.emitter == -1);
        GX_ASSERT_D(node.mesh == -1);
        GX_ASSERT_D(node.skin == -1);
        return true;
    }
    return false;
}

bool gearoenix::render::gltf::Lights::process(
    const int node_index, core::ecs::Entity* const parent,
    const core::job::EndCaller<>& end_callback) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_light(node_index)) {
        return false;
    }
    const auto& light_ext = node.extensions.begin()->second;
    GX_ASSERT_D(light_ext.IsObject());
    const auto light_index = light_ext.Get("light").GetNumberAsInt();
    auto light_ext_search = context.data.extensions.find(light_ext_name);
    GX_ASSERT_D(context.data.extensions.end() != light_ext_search);
    const auto& lights = light_ext_search->second.Get("lights");
    GX_ASSERT_D(lights.IsArray());
    const auto& light_info = lights.Get(light_index);
    GX_ASSERT_D(light_info.IsObject());
    const auto& json_light_colour = light_info.Get("color");
    GX_ASSERT_D(json_light_colour.IsArray());
    GX_ASSERT_D(3 == json_light_colour.ArrayLen());
    const auto colour_x = static_cast<float>(json_light_colour.Get(0).GetNumberAsDouble());
    const auto colour_y = static_cast<float>(json_light_colour.Get(1).GetNumberAsDouble());
    const auto colour_z = static_cast<float>(json_light_colour.Get(2).GetNumberAsDouble());
    GX_ASSERT_D(0.0f <= colour_x);
    GX_ASSERT_D(1.0f >= colour_x);
    GX_ASSERT_D(0.0f <= colour_y);
    GX_ASSERT_D(1.0f >= colour_y);
    GX_ASSERT_D(0.0f <= colour_z);
    GX_ASSERT_D(1.0f >= colour_z);
    const auto intensity = static_cast<float>(light_info.Get("intensity").GetNumberAsDouble() / 683.0);
    const auto colour = math::Vec3(colour_x, colour_y, colour_z) * intensity;
    const auto light_type = light_info.Get("type").Get<std::string>();
    GX_ASSERT_D(light_type == "directional");
    light::Manager::get().build_shadow_caster_directional(
        std::string(node.name), parent, 1024, 20.0f, 1.0f, 35.0f,
        core::job::EndCaller<core::ecs::EntityPtr>([this, node_index, e = end_callback, colour](auto&& l) {
            l->template get_component<light::Light>()->colour = colour;
            apply_transform(node_index, context, *l->template get_component<light::ShadowCasterDirectional>()->get_shadow_transform());
            (void)e;
        }));
    return true;
}
