#include "gx-rnd-gltf-light.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-builder.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-light.hpp"
#include "../light/gx-rnd-lt-manager.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
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
    const int node_index, physics::Transformation* const parent_transform,
    const core::job::EndCaller<>& gpu_end_callback, const core::job::EndCaller<>& entity_end_callback,
    const std::shared_ptr<scene::Builder>& scene_builder) const
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
    engine::Engine::get()->get_light_manager()->build_shadow_caster_directional(
        node.name, parent_transform, 1024, 20.0f, 1.0f, 35.0f,
        core::job::EndCallerShared<light::Builder>([this, sb = scene_builder, node_index, le = gpu_end_callback, colour](std::shared_ptr<light::Builder>&& lb) {
            lb->get_light().colour = colour;
            apply_transform(node_index, context, *lb->get_shadow_caster_directional()->get_shadow_transform());
            sb->add(std::move(lb));
            (void)le;
        }),
        core::job::EndCaller(entity_end_callback));
    return true;
}
