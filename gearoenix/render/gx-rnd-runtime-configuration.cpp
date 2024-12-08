#include "gx-rnd-runtime-configuration.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/gx-cr-string.hpp"
#include "../math/gx-math-numeric.hpp"
#include "engine/gx-rnd-eng-engine.hpp"
#include "imgui/gx-rnd-imgui-input-uint.hpp"
#include "imgui/gx-rnd-imgui-observer.hpp"

namespace {
constexpr auto entity_name = "gearoenix-render-runtime-configuration";
}

gearoenix::render::RuntimeConfiguration::RuntimeConfiguration(const core::ecs::entity_id_t id)
    : Component(core::ecs::ComponentType::create_index(this), entity_name, id)
    , runtime_resolution(FixedResolution {})
{
    set_runtime_reflection_radiance_resolution(runtime_reflection_radiance_resolution);
}

gearoenix::render::RuntimeConfiguration::~RuntimeConfiguration() = default;

void gearoenix::render::RuntimeConfiguration::set_runtime_reflection_radiance_resolution(const std::uint16_t value)
{
    runtime_reflection_radiance_resolution = value;
    runtime_reflection_radiance_levels = compute_radiance_mipmaps_count(value);
}

std::uint8_t gearoenix::render::RuntimeConfiguration::compute_radiance_mipmaps_count(const std::uint16_t value)
{
    return static_cast<std::uint8_t>(math::Numeric::floor_log2(value) - 3);
}

void gearoenix::render::RuntimeConfiguration::show_debug_gui()
{
    if (!ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        return;
    }

    Component::show_debug_gui();

    if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        return;
    }
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.999f);
    ImGui::TableNextColumn();

    ImGui::Text("Shadow Cascades Count: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(shadow_cascades_count);
    ImGui::TableNextColumn();

    ImGui::Text("Runtime Reflection Environment Resolution: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(runtime_reflection_environment_resolution);
    ImGui::TableNextColumn();

    ImGui::Text("Runtime Reflection Irradiance Resolution: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(runtime_reflection_irradiance_resolution);
    ImGui::TableNextColumn();

    ImGui::Text("Maximum CPU Render Memory Size: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(maximum_cpu_render_memory_size);
    ImGui::TableNextColumn();

    ImGui::Text("Maximum GPU Render Memory Size: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(maximum_gpu_render_memory_size);
    ImGui::TableNextColumn();

    ImGui::Text("Maximum GPU Buffer Size: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(maximum_gpu_buffer_size);
    ImGui::TableNextColumn();

    ImGui::Text("Maximum Dynamic Buffer Size: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(maximum_dynamic_buffer_size);
    ImGui::TableNextColumn();

    ImGui::Text("Brdflut Resolution: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(brdflut_resolution);
    ImGui::TableNextColumn();

    ImGui::Text("Runtime Reflection Radiance Resolution: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(runtime_reflection_radiance_resolution);
    ImGui::TableNextColumn();

    ImGui::Text("Runtime Reflection Radiance Levels: ");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(runtime_reflection_radiance_levels);
    ImGui::TableNextColumn();

    ImGui::Text("Runtime Resolution: ");
    ImGui::TableNextColumn();
    GX_IMGUI_OBSERVER(runtime_resolution, imgui_show);

    ImGui::EndTable();

    ImGui::TreePop();
}