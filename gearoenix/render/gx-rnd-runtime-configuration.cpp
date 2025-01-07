#include "gx-rnd-runtime-configuration.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../math/gx-math-numeric.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "engine/gx-rnd-eng-engine.hpp"
#include "imgui/gx-rnd-imgui-input-uint.hpp"
#include "imgui/gx-rnd-imgui-type-table.hpp"
#include "imgui/gx-rnd-imgui-type-tree.hpp"

namespace {
constexpr auto entity_name = "gearoenix-render-runtime-configuration";
}

void gearoenix::render::RuntimeConfiguration::write_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& stream,
    core::job::EndCaller<>&&) const
{
    stream->write_fail_debug(shadow_cascades_count);
    stream->write_fail_debug(runtime_reflection_environment_resolution);
    stream->write_fail_debug(runtime_reflection_irradiance_resolution);
    stream->write_fail_debug(maximum_cpu_render_memory_size);
    stream->write_fail_debug(maximum_gpu_render_memory_size);
    stream->write_fail_debug(maximum_gpu_buffer_size);
    stream->write_fail_debug(maximum_dynamic_buffer_size);
    stream->write_fail_debug(brdflut_resolution);
    stream->write_fail_debug(runtime_reflection_radiance_resolution);
    stream->write_fail_debug(runtime_reflection_radiance_levels);
    runtime_resolution.write(*stream);
}

void gearoenix::render::RuntimeConfiguration::update_in_io_context(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&&)
{
    s->read(shadow_cascades_count);
    s->read(runtime_reflection_environment_resolution);
    s->read(runtime_reflection_irradiance_resolution);
    s->read(maximum_cpu_render_memory_size);
    s->read(maximum_gpu_render_memory_size);
    s->read(maximum_gpu_buffer_size);
    s->read(maximum_dynamic_buffer_size);
    s->read(brdflut_resolution);
    s->read(runtime_reflection_radiance_resolution);
    s->read(runtime_reflection_radiance_levels);
    runtime_resolution.read(*s);
}

gearoenix::render::RuntimeConfiguration::RuntimeConfiguration(const core::ecs::entity_id_t id)
    : Component(core::ecs::ComponentType::create_index(this), entity_name, id)
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
    imgui::tree_scope(this, [this] {
        Component::show_debug_gui();
        imgui::table_scope("##gearoenix::render::RuntimeConfiguration::show_debug_gui", [this] {
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
            (void)runtime_resolution.show_debug_gui();
        });
    });
}