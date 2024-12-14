#include "gx-plt-runtime-configuration.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/gx-cr-string.hpp"
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../render/imgui/gx-rnd-imgui-input-uint.hpp"
#include "gx-plt-application.hpp"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace {
constexpr auto entity_name = "gearoenix-platform-runtime-configuration";
}

void gearoenix::platform::RuntimeConfiguration::write_in_io_context(
    std::shared_ptr<stream::Stream>&& stream,
    core::job::EndCaller<>&&) const
{
    stream->write_fail_debug(application_name);
    stream->write_fail_debug(fullscreen);
    stream->write_fail_debug(landscape);
    stream->write_fail_debug(window_width);
    stream->write_fail_debug(window_height);
    stream->write_fail_debug(window_resizing_event_interval_ms);
    stream->write_fail_debug(show_cursor);
    stream->write_fail_debug(vulkan_render_backend_enabled);
    stream->write_fail_debug(direct3dx_render_backend_enabled);
    stream->write_fail_debug(metal_render_backend_enabled);
    stream->write_fail_debug(opengl_render_backend_enabled);
}

void gearoenix::platform::RuntimeConfiguration::update_in_io_context(
    std::shared_ptr<stream::Stream>&& stream, core::job::EndCaller<>&&)
{
    stream->read(application_name);
    stream->read(fullscreen);
    stream->read(landscape);
    stream->read(window_width);
    stream->read(window_height);
    stream->read(window_resizing_event_interval_ms);
    stream->read(show_cursor);
    stream->read(vulkan_render_backend_enabled);
    stream->read(direct3dx_render_backend_enabled);
    stream->read(metal_render_backend_enabled);
    stream->read(opengl_render_backend_enabled);
}

gearoenix::platform::RuntimeConfiguration::RuntimeConfiguration(const core::ecs::entity_id_t id)
    : Component(core::ecs::ComponentType::create_index(this), entity_name, id)
    , application_name(GX_APPLICATION_NAME)
{
}

gearoenix::platform::RuntimeConfiguration::~RuntimeConfiguration() = default;

void gearoenix::platform::RuntimeConfiguration::show_debug_gui()
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

    ImGui::Text("Application Name:");
    ImGui::TableNextColumn();
    ImGui::InputText("##" GX_STRINGIFY(application_name), &application_name);
    ImGui::TableNextColumn();

    ImGui::Text("Fullscreen:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(fullscreen), &fullscreen);
    ImGui::TableNextColumn();

    ImGui::Text("Landscape:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(lanscape), &landscape);
    ImGui::TableNextColumn();

    ImGui::Text("Window Size:");
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Width:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    GX_IMGUI_VAR_INPUT_UINT(window_width);
    ImGui::SameLine();
    ImGui::Text("Height:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    GX_IMGUI_VAR_INPUT_UINT(window_height);
    ImGui::TableNextColumn();

    ImGui::Text("Window Resizing Event Interval MS:");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(window_resizing_event_interval_ms);
    ImGui::TableNextColumn();

    ImGui::Text("Show Cursor:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(show_cursor), &show_cursor);
    ImGui::TableNextColumn();

    ImGui::Text("Vulkan Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(vulkan_render_backend_enabled), &vulkan_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::Text("Direct3DX Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(direct3dx_render_backend_enabled), &direct3dx_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::Text("Metal Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(metal_render_backend_enabled), &metal_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::Text("OpenGL Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(opengl_render_backend_enabled), &opengl_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::EndTable();

    ImGui::TreePop();
}